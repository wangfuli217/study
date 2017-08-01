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
#include <iostream>
#include "object.h"
#include "concurrency_copy.h"

namespace gc {

// FIXME: 存在回收时新的对象覆盖老的未来得及回收的对象的bug？？？

ConcurrencyCopy::ConcurrencyCopy(void)
  : copy_mutex_()
  , copy_cond_(copy_mutex_)
  , collect_mutex_()
  , collect_cond_(collect_mutex_)
  , thread_([this]{ concurrency_closure(); }) {
  heaptr_ = new byte_t[kSemispaceSize * 2];
  CHAOS_CHECK(heaptr_ != nullptr, "create semispace failed");

  fromspace_ = heaptr_ + kSemispaceSize;
  tospace_ = heaptr_;
  allocptr_ = tospace_;

  thread_.start();
}

ConcurrencyCopy::~ConcurrencyCopy(void) {
  running_ = false;
  copy_cond_.notify_one();

  thread_.join();
}

void* ConcurrencyCopy::alloc(std::size_t n) {
  if (allocptr_ + n >= tospace_ + kSemispaceSize)
    collect();

  Chaos::ScopedLock<Chaos::Mutex> g(mutex_);

  auto* p = allocptr_;
  allocptr_ += n;
  return p;
}

void ConcurrencyCopy::concurrency_closure(void) {
  while (running_) {
    {
      Chaos::ScopedLock<Chaos::Mutex> g(copy_mutex_);
      while (running_ && !copying_)
        copy_cond_.wait();
      if (!running_)
        break;
    }

    collecting();
  }
}

BaseObject* ConcurrencyCopy::forward(BaseObject* from_ref) {
  auto* to_ref = from_ref->forward();
  if (to_ref == nullptr) {
    to_ref = copy(from_ref);
    ++object_counter_;
  }
  return Chaos::down_cast<BaseObject*>(to_ref);
}

BaseObject* ConcurrencyCopy::copy(BaseObject* from_ref) {
  auto* p = allocptr_;
  allocptr_ += from_ref->get_size();
  CHAOS_CHECK(allocptr_ <= tospace_ + kSemispaceSize, "out of memory");

  BaseObject* to_ref{};
  if (from_ref->is_int())
    to_ref = new (p) Int(std::move(*Chaos::down_cast<Int*>(from_ref)));
  else if (from_ref->is_pair())
    to_ref = new (p) Pair(std::move(*Chaos::down_cast<Pair*>(from_ref)));
  CHAOS_CHECK(to_ref != nullptr, "copy object failed");
  from_ref->set_forward(to_ref);

  worklist_.push_back(to_ref);
  return to_ref;
}

void ConcurrencyCopy::collecting(void) {
  auto old_count = object_counter_;

  object_counter_ = 0;
  {
    Chaos::ScopedLock<Chaos::Mutex> g(mutex_);
    // semispace flip
    std::swap(fromspace_, tospace_);
    allocptr_ = tospace_;

    for (auto& obj : roots_)
      obj = forward(obj);
  }
  collect_cond_.notify_one();

  while (true) {
    Chaos::ScopedLock<Chaos::Mutex> g(mutex_);
    if (worklist_.empty())
      break;

    auto* obj = worklist_.back();
    worklist_.pop_back();
    if (obj->is_pair()) {
      auto* pair = Chaos::down_cast<Pair*>(obj);
      auto* first = pair->first();
      if (first != nullptr)
        pair->set_first(forward(first));

      auto* second = pair->second();
      if (second != nullptr)
        pair->set_second(forward(second));
    }
  }

  std::cout
    << "[" << old_count << "] objects alive before collecting, "
    << "[" << object_counter_ << "] objects alive after collecting."
    << std::endl;
}

ConcurrencyCopy& ConcurrencyCopy::get_instance(void) {
  static ConcurrencyCopy ins;
  return ins;
}

void ConcurrencyCopy::collect(void) {
  copying_ = true;
  copy_cond_.notify_one();

  {
    Chaos::ScopedLock<Chaos::Mutex> g(collect_mutex_);
    collect_cond_.wait();
  }
}

BaseObject* ConcurrencyCopy::put_in(int value) {
  auto* obj = new (alloc(sizeof(Int))) Int();
  obj->set_value(value);

  roots_.push_back(obj);
  ++object_counter_;

  return obj;
}

BaseObject* ConcurrencyCopy::put_in(BaseObject* first, BaseObject* second) {
  auto* obj = new (alloc(sizeof(Pair))) Pair();
  if (first != nullptr)
    obj->set_first(first);
  if (second != nullptr)
    obj->set_second(second);

  roots_.push_back(obj);
  ++object_counter_;

  return obj;
}

BaseObject* ConcurrencyCopy::fetch_out(void) {
  Chaos::ScopedLock<Chaos::Mutex> g(mutex_);

  auto* obj = roots_.back();
  roots_.pop_back();
  return obj;
}

}
