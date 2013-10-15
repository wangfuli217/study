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
#ifndef __SELECT_HEADER_H__
#define __SELECT_HEADER_H__

#ifndef _WINDOWS_
# include <winsock2.h>
#endif
#include <map>
#include "locker.h"


struct EventHandler;
class Select {
  SpinLock spinlock_;
  fd_set rset_;
  fd_set wset_;
  std::map<int, EventHandler*> handlers_;

  Select(const Select&);
  Select& operator =(const Select&);
public:
  explicit Select(void);
  ~Select(void);

  inline std::map<int, EventHandler*>* handlers(void) {
    return &handlers_;
  }

  void Insert(EventHandler* eh, int ev);
  void Remove(EventHandler* eh);
  void AddEvent(EventHandler* eh, int ev);
  void DelEvent(EventHandler* eh, int ev);

  void Poll(void);
private:
  void InitSets(void);
  void DispatchEvent(fd_set* fds, int ev);
};

#endif  //! __SELECT_HEADER_H__
