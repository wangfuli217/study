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
#include <process.h>
#include "win_thread.h"



Thread::Thread(void)
  : thread_(NULL)
  , start_event_(NULL)
  , routine_(NULL)
  , argument_(NULL)
{
}

Thread::~Thread(void)
{
  Stop();
}

bool 
Thread::Start(void (*routine)(void*), void* argument)
{
  routine_ = routine;
  argument_ = argument;

  start_event_ = CreateEvent(NULL, TRUE, FALSE, NULL);
  if (NULL == start_event_)
    return false;

  thread_ = (HANDLE)_beginthreadex(NULL, 
      0, &Thread::Routine, this, 0, NULL);
  
  if (NULL != thread_) 
    WaitForSingleObject(start_event_, INFINITE);
  CloseHandle(start_event_);

  return (NULL != thread_);
}

void 
Thread::Stop(void)
{
  if (NULL != thread_) {
    WaitForSingleObject(thread_, INFINITE);

    CloseHandle(thread_);
    thread_ = NULL;
  }
}


UINT WINAPI 
Thread::Routine(void* argument)
{
  Thread* self = static_cast<Thread*>(argument);
  if (NULL == self)
    return 0;

  SetEvent(self->start_event_);

  if (NULL != self->routine_)
    self->routine_(self->argument_);

  return 0;
}
