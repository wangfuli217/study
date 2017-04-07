/*
 * Copyright (c) 2017 ASMlover. All rights reserved.
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
#include "njmem.h"
#include "njobject.h"
#include "gc_impl.h"

static NjGCObject* gcobj = NULL;

void
njord_initgc(NjGCType type) {
  if (type == GC_REFS)
    gcobj = &NjGC_Refs;
  else if (type == GC_MARK_SWEEP)
    gcobj = &NjGC_MarkS;
}

NjObject*
njord_new(void) {
  return Nj_GCFUN(gcobj)->tp_newvm();
}

void
njord_free(NjObject* vm) {
  Nj_GCFUN(gcobj)->tp_freevm(vm);
}

NjObject*
njord_pushint(NjObject* vm, int value) {
  return Nj_GCFUN(gcobj)->tp_pushint(vm, value);
}

NjObject*
njord_pushpair(NjObject* vm) {
  return Nj_GCFUN(gcobj)->tp_pushpair(vm);
}

void
njord_setpair(NjObject* pair, NjObject* head, NjObject* tail) {
  Nj_GCFUN(gcobj)->tp_setpair(pair, head, tail);
}

void
njord_pop(NjObject* vm) {
  Nj_GCFUN(gcobj)->tp_pop(vm);
}

void
njord_collect(NjObject* vm) {
  if (Nj_GCFUN(gcobj)->tp_collect != NULL)
    Nj_GCFUN(gcobj)->tp_collect(vm);
  else
    njmem_collect();
}
