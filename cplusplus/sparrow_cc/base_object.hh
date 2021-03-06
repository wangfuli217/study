// Copyright (c) 2019 ASMlover. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//  * Redistributions of source code must retain the above copyright
//    notice, this list ofconditions and the following disclaimer.
//
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in
//    the documentation and/or other materialsprovided with the
//    distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
#pragma once

#include "common.hh"

namespace sparrow {

enum class ObjType {
  CLASS,
  LIST,
  MAP,
  MODULE,
  RANGE,
  STRING,
  UPVALUE,
  FUNCTION,
  CLOSURE,
  INSTANCE,
  THREAD,
};

class VM;
class Value;

class ClassObject;
class ListObject;
class MapObject;
class ModuleObject;
class RangeObject;
class StringObject;
class UpvalueObject;
class FunctionObject;
class ClosureObject;
class InstanceObject;
class ThreadObject;

class BaseObject : private UnCopyable {
  ObjType type_{};
  bool marked_{};
  ClassObject* cls_{};
public:
  BaseObject(VM& vm, ObjType type, ClassObject* cls = nullptr);
  virtual ~BaseObject(void);

  inline ObjType type(void) const { return type_; }
  inline bool marked(void) const { return marked_; }
  inline ClassObject* cls(void) const { return cls_; }

  virtual bool is_equal(BaseObject* other) const { return true; }
  virtual sz_t hasher(void) const { return 0; }
};

}
