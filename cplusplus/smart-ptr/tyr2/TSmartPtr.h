// Copyright (c) 2016 ASMlover. All rights reserved.
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
#ifndef TYR_SMART_PTR_HEADER_H
#define TYR_SMART_PTR_HEADER_H

#include "TSharedCount.h"

namespace tyr {

template <typename T> class WeakPtr;

template <typename T>
class SmartPtr {
  T* px_;
  SharedCount pn_;

  typedef SmartPtr<T> SelfType;
public:
  SmartPtr(void)
    : px_(nullptr)
    , pn_() {
  }

  SmartPtr(std::nullptr_t)
    : px_(nullptr)
    , pn_() {
  }

  template <typename Y>
  explicit SmartPtr(Y* p)
    : px_(p)
    , pn_() {
    SharedCount(p).Swap(pn_);
  }

  template <typename Y, typename D>
  SmartPtr(Y* p, D d)
    : px_(p)
    , pn_(p, d) {
  }

  template <typename D>
  SmartPtr(std::nullptr_t p, D d)
    : px_(p)
    , pn_(p, d) {
  }

  SmartPtr(const SmartPtr& r)
    : px_(r.px_)
    , pn_(r.pn_) {
  }

  template <typename Y>
  SmartPtr(const SmartPtr<Y>& r)
    : px_(r.px_)
    , pn_(r.pn_) {
  }

  template <typename Y>
  explicit SmartPtr(const WeakPtr<Y>& r)
    : px_(r.px_)
    , pn_(r.pn_) {
  }

  SmartPtr& operator=(const SmartPtr& r) {
    SelfType(r).Swap(*this);
    return *this;
  }

  template <typename Y>
  SmartPtr& operator=(const SmartPtr<Y>& r) {
    SelfType(r).Swap(*this);
    return *this;
  }

  SmartPtr(SmartPtr&& r)
    : px_(r.px_)
    , pn_() {
    pn_.Swap(r.pn_);
    r.px_ = nullptr;
  }

  template <typename Y>
  SmartPtr(SmartPtr<Y>&& r)
    : px_(r.px_)
    , pn_() {
    pn_.Swap(r.pn_);
    r.px_ = nullptr;
  }

  SmartPtr& operator=(SmartPtr&& r) {
    SelfType(static_cast<SmartPtr&&>(r)).Swap(*this);
    return *this;
  }

  template <typename Y>
  SmartPtr& operator=(SmartPtr<Y>&& r) {
    SelfType(static_cast<SmartPtr<Y>&&>(r)).Swap(*this);
    return *this;
  }

  SmartPtr& operator=(std::nullptr_t) {
    SelfType().Swap(*this);
    return *this;
  }

  void Reset(void) {
    SelfType().Swap(*this);
  }

  template <typename Y>
  void Reset(Y* p) {
    SelfType(p).Swap(*this);
  }

  template <typename Y, typename D>
  void Reset(Y* p, D d) {
    SelfType(p, d).Swap(*this);
  }

  template <typename Y>
  void Reset(const SmartPtr<Y>& r) {
    SelfType(r).Swap(*this);
  }

  void Swap(SmartPtr& r) {
    std::swap(px_, r.px_);
    pn_.Swap(r.pn_);
  }
};

}

#endif // TYR_SMART_PTR_HEADER_H
