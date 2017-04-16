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
#include <string.h>
#include "gc_impl.h"
#include "njlog.h"
#include "njmem.h"

#define MAX_STACK         (1024)
#define INIT_GC_THRESHOLD (8)
#define MAX_GC_THRESHOLD  (1024)
#define Nj_BIT(ob)        (gc_bitmap[_hash_index(ob)])

typedef enum _marked {
  UNMARKED,
  MARKED,
} NjMarked;

typedef struct _vm {
  NjObject_HEAD;

  NjObject* stack[MAX_STACK];
  Nj_int_t stackcnt;

  NjObject* startobj;
  Nj_int_t objcnt;
  Nj_int_t maxobj;
} NjVMObject;

static Nj_uchar_t gc_bitmap[MAX_GC_THRESHOLD];

static inline int
_hash_index(NjObject* obj) {
  Nj_size_t h = 1315423911;
  h ^= ((h << 5) + (Nj_size_t)obj + (h >> 2));
  return h % MAX_GC_THRESHOLD;
}

static void
_njbitmap_push(NjVMObject* vm, NjObject* obj) {
  Nj_CHECK(vm->stackcnt < MAX_STACK, "VM stack overflow");
  vm->stack[vm->stackcnt++] = obj;
}

static NjObject*
_njbitmap_pop(NjVMObject* vm) {
  Nj_CHECK(vm->stackcnt > 0, "VM stack underflow");
  return vm->stack[--vm->stackcnt];
}

static void
_njbitmap_mark(NjObject* obj) {
  if (Nj_BIT(obj) == MARKED)
    return;

  Nj_BIT(obj) = MARKED;
  if (obj->ob_type == &NjPair_Type) {
    NjObject* head = njord_pairgetter(obj, "head");
    _njbitmap_mark(head);
    NjObject* tail = njord_pairgetter(obj, "tail");
    _njbitmap_mark(tail);
  }
}

static void
_njbitmap_mark_all(NjVMObject* vm) {
  memset(gc_bitmap, 0, sizeof(gc_bitmap));
  for (int i = 0; i < vm->stackcnt; ++i)
    _njbitmap_mark(vm->stack[i]);
}

static void
_njbitmap_sweep(NjVMObject* vm) {
  NjObject** startobj = &vm->startobj;
  while (*startobj != NULL) {
    if (Nj_BIT(*startobj) == UNMARKED) {
      NjObject* unmarked = *startobj;
      *startobj = ((NjVarObject*)unmarked)->next;
      njlog_debug("NjObject<%p, '%s'> collected\n",
          unmarked, unmarked->ob_type->tp_name);
      njord_freeobj(unmarked, 0);
      --vm->objcnt;
    }
    else {
      startobj = &((NjVarObject*)*startobj)->next;
    }
  }
}

static void
njbitmap_collect(NjObject* _vm) {
  NjVMObject* vm = (NjVMObject*)_vm;
  Nj_int_t old_objcnt = vm->objcnt;

  _njbitmap_mark_all(vm);
  _njbitmap_sweep(vm);

  if (vm->maxobj < MAX_GC_THRESHOLD) {
    vm->maxobj = vm->objcnt << 1;
    if (vm->maxobj > MAX_GC_THRESHOLD)
      vm->maxobj = MAX_GC_THRESHOLD;
  }

  njlog_info("<%s> collected [%d] objects, [%d] remaining.\n",
      vm->ob_type->tp_name, old_objcnt - vm->objcnt, vm->objcnt);
}

static NjObject*
njbitmap_newvm(void) {
  NjVMObject* vm = (NjVMObject*)njmem_malloc(sizeof(NjVMObject));
  Nj_CHECK(vm != NULL, "create VM failed");

  vm->ob_type = &NjBitmap_Type;
  vm->stackcnt = 0;
  vm->startobj = NULL;
  vm->objcnt = 0;
  vm->maxobj = INIT_GC_THRESHOLD;

  return (NjObject*)vm;
}

static void
njbitmap_freevm(NjObject* vm) {
  njbitmap_collect(vm);
  njmem_free(vm, sizeof(NjVMObject));
}

static NjObject*
njbitmap_pushint(NjObject* _vm, int value) {
  NjVMObject* vm = (NjVMObject*)_vm;
  if (vm->objcnt >= vm->maxobj)
    njbitmap_collect(_vm);

  NjIntObject* obj = (NjIntObject*)njord_newint(0, value, NULL, NULL);
  obj->next = vm->startobj;
  vm->startobj = (NjObject*)obj;
  ++vm->objcnt;
  _njbitmap_push(vm, (NjObject*)obj);

  return (NjObject*)obj;
}

static NjObject*
njbitmap_pushpair(NjObject* _vm) {
  NjVMObject* vm = (NjVMObject*)_vm;
  if (vm->objcnt >= vm->maxobj)
    njbitmap_collect(_vm);

  NjObject* tail = _njbitmap_pop(vm);
  NjObject* head = _njbitmap_pop(vm);
  NjPairObject* obj = (NjPairObject*)njord_newpair(0, head, tail, NULL, NULL);
  obj->next = vm->startobj;
  vm->startobj = (NjObject*)obj;
  ++vm->objcnt;
  _njbitmap_push(vm, (NjObject*)obj);

  return (NjObject*)obj;
}

static void
njbitmap_setpair(NjObject* obj, NjObject* head, NjObject* tail) {
  if (head != NULL)
    njord_pairsetter(obj, "head", head);

  if (tail != NULL)
    njord_pairsetter(obj, "tail", tail);
}

static void
njbitmap_pop(NjObject* vm) {
  _njbitmap_pop((NjVMObject*)vm);
}

static NjGCMethods gc_methods = {
  njbitmap_newvm, /* gc_newvm */
  njbitmap_freevm, /* gc_freevm */
  njbitmap_pushint, /* gc_pushint */
  njbitmap_pushpair, /* gc_pushpair */
  njbitmap_setpair, /* gc_setpair */
  njbitmap_pop, /* gc_pop */
  njbitmap_collect, /* gc_collect */
};

NjTypeObject NjBitmap_Type = {
  NjObject_HEAD_INIT(&NjType_Type),
  "marks_bitmap_gc", /* tp_name */
  0, /* tp_print */
  0, /* tp_setter */
  0, /* tp_getter */
  (NjGCMethods*)&gc_methods, /* tp_gc */
};