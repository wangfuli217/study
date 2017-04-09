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
#include <stdio.h>
#include <stdlib.h>
#include "gc_impl.h"
#include "njmem.h"

#define MAX_STACK         (1024)
#define INIT_GC_THRESHOLD (8)
#define Nj_ASGC(ob)       ((GCHead*)(ob) - 1)

typedef enum _marked {
  UNMARKED,
  MARKED,
} NjMarked;

typedef struct _gc {
  Nj_uchar_t marked;
} GCHead;

typedef struct _vm {
  NjObject_HEAD;

  NjObject* stack[MAX_STACK];
  int stackcnt;

  NjObject* startobj;
  int objcnt;
  int maxobj;
} NjVMObject;

static void
_njord_push(NjVMObject* vm, NjObject* obj) {
  Nj_CHECK(vm->stackcnt < MAX_STACK, "VM stack overflow");
  vm->stack[vm->stackcnt++] = obj;
}

static NjObject*
_njord_pop(NjVMObject* vm) {
  Nj_CHECK(vm->stackcnt > 0, "VM stack underflow");
  return vm->stack[--vm->stackcnt];
}

static void
_njord_mark(NjObject* obj) {
  if (Nj_ASGC(obj)->marked == MARKED)
    return;

  Nj_ASGC(obj)->marked = MARKED;
  if (obj->ob_type == &NjPair_Type) {
    _njord_mark(((NjPairObject*)obj)->head);
    _njord_mark(((NjPairObject*)obj)->tail);
  }
}

static void
_njord_mark_all(NjVMObject* vm) {
  for (int i = 0; i < vm->stackcnt; ++i)
    _njord_mark(vm->stack[i]);
}

static void
_njord_sweep(NjVMObject* vm) {
  NjObject** startobj = &vm->startobj;
  while (*startobj != NULL) {
    if (Nj_ASGC(*startobj)->marked == UNMARKED) {
      NjObject* unmarked = *startobj;
      startobj = &((NjVarObject*)unmarked)->next;
      fprintf(stdout, "NjObject<0x%p, '%s'> collected\n",
          unmarked, unmarked->ob_type->tp_name);
      njord_free_object(unmarked, sizeof(GCHead));
      --vm->objcnt;
    }
    else {
      Nj_ASGC(*startobj)->marked = UNMARKED;
      startobj = &((NjVarObject*)*startobj)->next;
    }
  }
}

static void
njmarks_collect(NjObject* vm) {
  NjVMObject* _vm = (NjVMObject*)vm;
  int old_objcnt = _vm->objcnt;

  _njord_mark_all(_vm);
  _njord_sweep(_vm);

  _vm->maxobj = (int)(old_objcnt * 1.5);
  fprintf(stdout, "NjGC_MarkS collected [%d] objects, [%d] remaining.\n",
      old_objcnt - _vm->objcnt, _vm->objcnt);
}

static NjObject*
_njord_new_object(NjVMObject* vm, NjVarType type) {
  if (vm->objcnt >= vm->maxobj)
    njmarks_collect((NjObject*)vm);

  NjObject* obj = (NjObject*)njord_new_object(type, sizeof(GCHead));
  Nj_ASGC(obj)->marked = UNMARKED;
  ((NjVarObject*)obj)->next = vm->startobj;
  vm->startobj = obj;
  ++vm->objcnt;

  return obj;
}

static NjObject*
njmarks_newvm(void) {
  NjVMObject* vm = (NjVMObject*)njmem_malloc(sizeof(NjVMObject));
  Nj_CHECK(vm != NULL, "create VM failed");

  vm->ob_type = &NjMarks_Type;
  vm->stackcnt = 0;
  vm->startobj = NULL;
  vm->objcnt = 0;
  vm->maxobj = INIT_GC_THRESHOLD;

  return (NjObject*)vm;
}

static void
njmarks_freevm(NjObject* vm) {
  njmarks_collect(vm);
  njmem_free(vm, sizeof(NjVMObject));
}

static NjObject*
njmarks_pushint(NjObject* vm, int value) {
  NjIntObject* obj = (NjIntObject*)_njord_new_object((NjVMObject*)vm, VAR_INT);
  obj->value = value;
  _njord_push((NjVMObject*)vm, (NjObject*)obj);

  return (NjObject*)obj;
}

static NjObject*
njmarks_pushpair(NjObject* vm) {
  NjPairObject* obj = (NjPairObject*)_njord_new_object(
      (NjVMObject*)vm, VAR_PAIR);
  obj->tail = _njord_pop((NjVMObject*)vm);
  obj->head = _njord_pop((NjVMObject*)vm);
  _njord_push((NjVMObject*)vm, (NjObject*)obj);

  return (NjObject*)obj;
}

static void
njmarks_setpair(NjObject* pair, NjObject* head, NjObject* tail) {
  if (head != NULL)
    ((NjPairObject*)pair)->head = head;

  if (tail != NULL)
    ((NjPairObject*)pair)->tail = tail;
}

static void
njmarks_pop(NjObject* vm) {
  _njord_pop((NjVMObject*)vm);
}

static NjGCMethods gc_methods = {
  njmarks_newvm, /* gc_newvm */
  njmarks_freevm, /* gc_freevm */
  njmarks_pushint, /* gc_pushint */
  njmarks_pushpair, /* gc_pushpair */
  njmarks_setpair, /* gc_setpair */
  njmarks_pop, /* gc_pop */
  njmarks_collect, /* gc_collect */
};

NjTypeObject NjMarks_Type = {
  NjObject_HEAD_INIT(&NjType_Type),
  "marks_gc", /* tp_name */
  0, /* tp_print */
  (NjGCMethods*)&gc_methods, /* tp_gc */
};
