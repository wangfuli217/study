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

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include "common.h"
#include "value.h"

namespace lox {

class Chunk;
class Object;
class StringObject;

enum InterpretRet {
  OK,
  COMPILE_ERROR,
  RUNTIME_ERROR
};

class VM : private UnCopyable {
  static constexpr std::size_t kDefaultCap = 256;

  Chunk& chunk_;
  int ip_{};
  std::vector<Value> stack_;
  std::vector<Object*> objects_;
  std::unordered_map<std::uint32_t, StringObject*> strings_;
  std::unordered_map<std::uint32_t, Value> globals_;

  void reset_stack(void) { stack_.clear(); }
  void push(const Value& value) { stack_.push_back(value); }
  Value pop(void) {
    auto value = stack_.back();
    stack_.pop_back();
    return value;
  }
  Value peek(int distance = 0) const { return stack_[stack_.size() - 1 - distance]; }

  void runtime_error(const char* format, ...);
public:
  VM(Chunk& c);
  ~VM(void);

  void put_in(Object* o) { objects_.push_back(o); }
  void put_in(std::uint32_t code, StringObject* s) { strings_[code] = s; }
  StringObject* fetch_out(std::uint32_t code) const;

  InterpretRet interpret(void);
  InterpretRet run(void);

  InterpretRet interpret(const std::string& source_bytes);
};

VM* global_vm(void);

}
