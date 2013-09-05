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
#ifndef __CONDITION_HEADER_H__
#define __CONDITION_HEADER_H__ 


struct cond_t {
  size_t            waiters_count;
  CRITICAL_SECTION  waiters_count_lock;
  HANDLE            signal_event;
  HANDLE            broadcast_event;
};

class condition_t : noncopyable {
  cond_t cond_;
  mutex_t& mutex_;
public:
  condition_t(mutex_t& mutex);
  ~condition_t(void);

  void wait(void);
  bool timedwait(unsigned int millitm);
  void notify(void);
  void notify_all(void);

private:
  int cond_init(cond_t* cond);
  void cond_destroy(cond_t* cond);
  void cond_signal(cond_t* cond);
  void cond_broadcast(cond_t* cond);
  void cond_wait(cond_t* cond, mutex_t* mutex);
  int cond_timedwait(cond_t* cond, mutex_t* mutex, unsigned int timeout);
  int wait_helper(cond_t* cond, mutex_t* mutex, unsigned int timeout);
};

#endif  //! __CONDITION_HEADER_H__
