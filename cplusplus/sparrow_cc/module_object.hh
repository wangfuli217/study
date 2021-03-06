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

#include <vector>
#include "base_object.hh"
#include "string_object.hh"
#include "value.hh"

namespace sparrow {

class Value;
class StringObject;

class ModuleObject final : public BaseObject {
  StringObject* name_{};
  std::vector<str_t> var_names_;
  std::vector<Value> var_values_;

  ModuleObject(VM& vm, const str_t& module_name);
  virtual ~ModuleObject(void);
public:
  inline StringObject* name(void) const { return name_; }
  inline const char* name_cstr(void) const { return name_->c_str(); }
  inline const Value& get_value(int i) const { return var_values_[i]; }

  int index_of(const str_t& name) const;
  int append_var(const str_t& name, const Value& value);
  void assign_var(int index, const Value& value);

  static ModuleObject* create(VM& vm, const str_t& module_name) {
    return new ModuleObject(vm, module_name);
  }
};

}
