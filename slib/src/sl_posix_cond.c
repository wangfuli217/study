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
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include "sl_cond.h"


#undef NANOSEC
#define NANOSEC     ((uint64_t)1e9)



int 
sl_cond_init(sl_cond_t* cond, sl_mutex_t* mutex)
{
  pthread_condattr_t attr;

  cond->mutex = mutex;
  if (0 != pthread_condattr_init(&attr))
    return -1;
  if (0 != pthread_condattr_setclock(&attr, CLOCK_MONOTONIC))
    goto Exit2;

  if (0 != pthread_cond_init(&cond->cond, &attr))
    goto Exit2;
  if (0 != pthread_condattr_destroy(&attr))
    goto Exit;

  return 0;

Exit:
  pthread_cond_destroy(&cond->cond);
Exit2:
  pthread_condattr_destroy(&attr);
  return -1;
}

void 
sl_cond_destroy(sl_cond_t* cond)
{
  if (0 != pthread_cond_destroy(&cond->cond))
    abort();
}

void 
sl_cond_signal(sl_cond_t* cond)
{
  if (0 != pthread_cond_signal(&cond->cond))
    abort();
}

void 
sl_cond_broadcast(sl_cond_t* cond)
{
  if (0 != pthread_cond_broadcast(&cond->cond))
    abort();
}

void 
sl_cond_wait(sl_cond_t* cond)
{
  if (0 != pthread_cond_wait(&cond->cond, cond->mutex))
    abort();
}

int 
sl_cond_timedwait(sl_cond_t* cond, unsigned int millitm)
{
  uint64_t timeout = (uint64_t)(millitm * 1e6);
  struct timespec ts;
  int result;

  clock_gettime(CLOCK_MONOTONIC, &ts);
  timeout += (((uint64_t)ts.tv_sec) * NANOSEC + ts.tv_nsec);
  ts.tv_sec = timeout / NANOSEC;
  ts.tv_nsec = timeout % NANOSEC;
  result = pthread_cond_timedwait(&cond->cond, cond->mutex, &ts);

  if (0 == result)
    return 0;
  if (ETIMEDOUT == result)
    return -1;

  abort();
  return -1;
}
