//! Copyright (c) 2013 ASMlover. All rights reserved.
//!
//! Redistribution and use in source and binary forms, with or without
//! modification, are permitted provided that the following conditions
//! are met:
//!
//!  * Redistributions of source code must retain the above copyright
//!    notice, this list ofconditions and the following disclaimer.
//!
//!  * Redistributions in binary form must reproduce the above copyright
//!    notice, this list of conditions and the following disclaimer in
//!    the documentation and/or other materialsprovided with the
//!    distribution.
//!
//! THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//! "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//! LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
//! FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//! COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//! INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//! BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//! LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//! CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//! LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
//! ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//! POSSIBILITY OF SUCH DAMAGE.
#if defined(_WINDOWS_) || defined(_MSC_VER)
# ifndef _WINDOWS_
#   include <winsock2.h>
# endif
# define EWOULDBLOCK  WSAEWOULDBLOCK
#elif defined(__linux__)
# include <sys/types.h>
# include <sys/select.h>
# include <unistd.h>
#endif
#include "net.h"
#include "logging.h"
#include "socket.h"
#include "select_poll.h"




SelectPoll::SelectPoll(void)
  : rbytes_(kDefaultBuffer)
  , wbytes_(kDefaultBuffer)
  , handler_(NULL)
  , spinlock_()
{
  connectors_.clear();
  FD_ZERO(&rset_);
  FD_ZERO(&wset_);
}

SelectPoll::~SelectPoll(void)
{
  CloseAll();
}

void 
SelectPoll::CloseAll(void)
{
  if (NULL == handler_)
    return;

  LockerGuard<SpinLock> guard(spinlock_);
  std::map<int, std::pair<int, Socket*> >::iterator it;
  for (it = connectors_.begin(); it != connectors_.end(); ++it) {
    handler_->CloseEvent(it->second.second);
    it->second.second->Close();
    delete it->second.second;
  }
  connectors_.clear();
}

bool 
SelectPoll::Insert(int fd, int ev)
{
  if (NULL == handler_)
    return false;

  LockerGuard<SpinLock> guard(spinlock_);
  std::map<int, std::pair<int, Socket*> >::iterator it;
  it = connectors_.find(fd);
  if (it == connectors_.end()) {
    Socket* s = new Socket();
    if (NULL == s) {
      LOG_WARN("new Socket failed\n");
      return false;
    }
    s->Attach(fd);
    s->SetNonBlock();
    s->SetKeepAlive(true);
    s->SetSelfReadBuffer(rbytes_);
    s->SetSelfWriteBuffer(wbytes_);
    
    connectors_[fd] = std::make_pair<int, Socket*>(ev, s);
  }

  return true;
}

void 
SelectPoll::Remove(int fd)
{
  if (NULL == handler_)
    return;

  LockerGuard<SpinLock> guard(spinlock_);
  std::map<int, std::pair<int, Socket*> >::iterator it;
  it = connectors_.find(fd);
  if (it != connectors_.end()) {
    handler_->CloseEvent(it->second.second);
    it->second.second->Close();
    delete it->second.second;
    connectors_.erase(it);
  }
}

bool 
SelectPoll::AddEvent(int fd, int ev)
{
  LockerGuard<SpinLock> guard(spinlock_);
  std::map<int, std::pair<int, Socket*> >::iterator it;
  it = connectors_.find(fd);
  if (it == connectors_.end())
    return false;

  it->second.first |= ev;

  return true;
}

bool 
SelectPoll::DelEvent(int fd, int ev)
{
  LockerGuard<SpinLock> guard(spinlock_);
  std::map<int, std::pair<int, Socket*> >::iterator it;
  it = connectors_.find(fd);
  if (it == connectors_.end())
    return false;

  it->second.first &= ~ev;

  return true;
}

Socket* 
SelectPoll::GetConnector(int fd)
{
  Socket* s = NULL;
  {
    LockerGuard<SpinLock> guard(spinlock_);
    std::map<int, std::pair<int, Socket*> >::iterator it;
    it = connectors_.find(fd);
    if (it != connectors_.end())
      s = it->second.second;
  }
  
  return s;
}

bool 
SelectPoll::Polling(int millitm)
{
  if (NULL == handler_)
    return false;

  struct timeval timeout;
  if (-1 == millitm) {
    timeout.tv_sec = 0;
    timeout.tv_usec = 500;
  }
  else {
    timeout.tv_sec = millitm / 1000;
    timeout.tv_usec = millitm % 1000 * 1000;
  }

  int max_fd;
  InitSets(&max_fd);

  int ret = select(max_fd + 1, &rset_, &wset_, NULL, &timeout);
  if (kNetTypeError == ret || 0 == ret)
    return false;

  DispatchEvents();

  return true;
}

void 
SelectPoll::InitSets(int* max_fd)
{
  FD_ZERO(&rset_);
  FD_ZERO(&wset_);
  *max_fd = 0;

  LockerGuard<SpinLock> guard(spinlock_);
  std::map<int, std::pair<int, Socket*> >::iterator it;
  for (it = connectors_.begin(); it != connectors_.end();) {
    if (kNetTypeInval == it->second.second->fd()) {
      delete it->second.second;
      connectors_.erase(it++);
    }
    else {
      if (it->second.first & kEventTypeRead)
        FD_SET(it->first, &rset_);

      if (it->second.first & kEventTypeWrite)
        FD_SET(it->first, &wset_);

      if (it->first > *max_fd)
        *max_fd = it->first;

      ++it;
    }
  }
}

void 
SelectPoll::DispatchEvents(void)
{
  if (NULL == handler_)
    return;

  int fd;
  Socket* s;
  LockerGuard<SpinLock> guard(spinlock_);
  std::map<int, std::pair<int, Socket*> >::iterator it;
  for (it = connectors_.begin(); it != connectors_.end(); ++it) {
    fd = it->first;
    s = it->second.second;

    if (NULL == s)
      continue;

    if (FD_ISSET(fd, &rset_)) {
      int read_bytes = s->DealWithSyncRead();
      if (read_bytes > 0) {
        handler_->ReadEvent(s);
      }
      else if (0 == read_bytes) {
        handler_->CloseEvent(s);
        s->Close();
      }
      else {
        if (EWOULDBLOCK != NErrno()) {
          handler_->CloseEvent(s);
          s->Close();
        }
      }
    }

    if (FD_ISSET(fd, &wset_)) {
      int write_bytes = s->DealWithSyncWrite();
      if (write_bytes > 0)
        handler_->WriteEvent(s);
    }
  }
}
