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
#include <cstring>
#include "object.hh"

namespace lvm {

Object::Object(ObjType t)
  : type_(t) {
}

StringObject* Object::as_string(void) const {
  return dynamic_cast<StringObject*>(const_cast<Object*>(this));
}

const char* Object::as_cstring(void) const {
  return as_string()->c_str();
}

StringObject::StringObject(void)
  : Object(ObjType::STRING) {
}

StringObject::~StringObject(void) {
  release_object();
}

void StringObject::release_object(void) {
  if (chars_ != nullptr) {
    delete [] chars_;
    chars_ = nullptr;
  }
  length_ = 0;
}

StringObject::StringObject(const char* s)
  : Object(ObjType::STRING)
  , length_(static_cast<int>(std::strlen(s)))
  , chars_(new char[length_ + 1]) {
  std::memcpy(chars_, s, length_);
  chars_[length_] = 0;
}

StringObject::StringObject(const char* s, int n)
  : Object(ObjType::STRING)
  , length_(n)
  , chars_(new char[length_ + 1]) {
  std::memcpy(chars_, s, length_);
  chars_[length_] = 0;
}

StringObject::StringObject(const std::string& s)
  : Object(ObjType::STRING)
  , length_(static_cast<int>(s.size()))
  , chars_(new char[length_ + 1]) {
  std::memcpy(chars_, s.data(), length_);
  chars_[length_] = 0;
}

StringObject::StringObject(const StringObject& s)
  : Object(ObjType::STRING)
  , length_(s.length_)
  , chars_(new char[length_ + 1]) {
  std::memcpy(chars_, s.chars_, length_);
  chars_[length_] = 0;
}

StringObject::StringObject(StringObject&& s)
  : Object(ObjType::STRING) {
  std::swap(length_, s.length_);
  std::swap(chars_, s.chars_);
}

StringObject& StringObject::operator=(const std::string& s) {
  return reset(s), *this;
}

StringObject& StringObject::operator=(const StringObject& s) {
  if (this != &s) {
    if (chars_ != nullptr)
      delete [] chars_;

    length_ = s.length_;
    chars_ = new char[length_ + 1];
    std::memcpy(chars_, s.chars_, length_);
    chars_[length_] = 0;
  }
  return *this;
}

StringObject& StringObject::operator=(StringObject&& s) {
  if (this != &s) {
    release_object();

    std::swap(length_, s.length_);
    std::swap(chars_, s.chars_);
  }
  return *this;
}

void StringObject::reset(void) {
  release_object();
}

void StringObject::reset(const char* s) {
  reset(s, static_cast<int>(std::strlen(s)));
}

void StringObject::reset(const char* s, int n) {
  release_object();

  length_ = n;
  chars_ = new char[length_ + 1];
  std::memcpy(chars_, s, length_);
  chars_[length_] = 0;
}

void StringObject::reset(const std::string& s) {
  reset(s.data(), static_cast<int>(s.size()));
}

void StringObject::reset(const StringObject& s) {
  if (this != &s) {
    if (chars_ != nullptr)
      delete [] chars_;

    length_ = s.length_;
    chars_ = new char[length_ + 1];
    std::memcpy(chars_, s.chars_, length_);
    chars_[length_] = 0;
  }
}

void StringObject::reset(StringObject&& s) {
  if (this != &s) {
    release_object();

    std::swap(length_, s.length_);
    std::swap(chars_, s.chars_);
  }
}

bool StringObject::is_equal(const Object* r) const {
  if (this == r)
    return true;

  auto* o = r->as_string();
  return o != nullptr && length_ == o->length_
    && std::memcmp(chars_, o->chars_, length_) == 0;
}

bool StringObject::is_truthy(void) const {
  return chars_ != nullptr && length_ != 0;
}

std::string StringObject::stringify(void) const {
  return chars_;
}

}
