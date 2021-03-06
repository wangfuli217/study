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

#include <functional>
#include "base_object.hh"

namespace sparrow {

class VM;
class Value;
class ClosureObject;
class StringObject;

typedef bool (*PrimitiveFn)(VM* vm, Value* args);

enum class MethodType {
  NONE,
  PRIMITIVE,
  SCRIPT,
  FUNCALL,
};

struct Method {
  MethodType type{MethodType::NONE};
  union {
    PrimitiveFn prim_fn;
    ClosureObject* closure;
  } as{};
};

class ClassObject : public BaseObject {
  ClassObject* superclass_{};
  u32_t field_num_{};
  std::vector<Method> methods_;
  StringObject* name_{};

  ClassObject(VM& vm, u32_t field_num, const str_t& name);
  virtual ~ClassObject(void);
public:
  inline u32_t field_num(void) const { return field_num_; }

  virtual sz_t hasher(void) const override;

  static ClassObject* create_raw(VM& vm, u32_t field_num, const str_t& name) {
    return new ClassObject(vm, field_num, name);
  }
};

union Bit64 {
  u64_t b64;
  u32_t b32[2];
  double num;
};

inline u32_t hash_numeric(double d) {
  Bit64 b64;
  b64.num = d;
  return b64.b32[0] ^ b64.b32[1];
}

// fnv-1a hashing
inline u32_t hash_string(const char* s, sz_t n) {
  u32_t hash_code = 2166136261;
  for (sz_t i = 0; i < n; ++i) {
    hash_code ^= s[i];
    hash_code *= 16777619;
  }
  return hash_code;
}

}
