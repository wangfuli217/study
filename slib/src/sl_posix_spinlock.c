/*
 * Copyright (c) 2013 ASMlover. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list ofconditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materialsprovided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <errno.h>
#include <stdlib.h>
#include "sl_spinlock.h"



int 
sl_spinlock_init(sl_spinlock_t* spinlock)
{
  return pthread_spin_init(spinlock, 0);
}

void 
sl_spinlock_destroy(sl_spinlock_t* spinlock)
{
  if (0 != pthread_spin_destroy(spinlock))
    abort();
}

void 
sl_spinlock_lock(sl_spinlock_t* spinlock)
{
  if (0 != pthread_spin_lock(spinlock))
    abort();
}

int 
sl_spinlock_trylock(sl_spinlock_t* spinlock)
{
  int ret = pthread_spin_trylock(spinlock);
  if (0 != ret && EBUSY != ret && EAGAIN != ret)
    abort();

  return ret;
}

void 
sl_spinlock_unlock(sl_spinlock_t* spinlock)
{
  if (0 != pthread_spin_unlock(spinlock))
    abort();
}
