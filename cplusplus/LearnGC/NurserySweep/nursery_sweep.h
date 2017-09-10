// Copyright (c) 2017 ASMlover. All rights reserved.
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

#include <list>
#include <stack>
#include <vector>
#include <unordered_set>
#include <Chaos/Base/UnCopyable.h>

namespace gc {

class BaseObject;

class NurserySweep : private Chaos::UnCopyable {
  std::vector<BaseObject*> roots_;
  std::list<BaseObject*> objects_;
  std::list<BaseObject*> record_objects_;
  std::unordered_set<BaseObject*> nursery_;
  static constexpr std::size_t kMaxObjects = 1024;

  NurserySweep(void) = default;
  ~NurserySweep(void) = default;

  void inc_nursery(BaseObject* ref);
  void dec_nursery(BaseObject* ref);
  void write_pair(BaseObject* p, BaseObject* obj, bool is_first = true);
  BaseObject* create_object(std::uint8_t type);
  void roots_nursery(void);
  void scan_nursery(void);
  void sweep_nursery(void);
  void roots_tracing(std::stack<BaseObject*>& trace_objects);
  void scan_tracing(std::stack<BaseObject*>& trace_objects);
  void sweep_tracing(void);
public:
  static NurserySweep& get_instance(void);

  void collect_nursery(void);
  void collect(void);
  BaseObject* put_in(int value);
  BaseObject* put_in(BaseObject* first = nullptr, BaseObject* second = nullptr);
  BaseObject* fetch_out(void);
};

}
