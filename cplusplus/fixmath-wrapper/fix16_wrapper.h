// Copyright (c) 2018 ASMlover. All rights reserved.
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
#include <sstream>
#include <string>
#include <libfixmath/fixmath.h>
#include "fixmath_utils.h"

namespace fixmath_wrapper {

class fix16 : public Copyable {
  fix16_t value_{};
public:
  fix16(void) noexcept {
  }

  fix16(fix16_t v) noexcept
    : value_(v) {
  }

  fix16(std::int16_t v) noexcept
    : value_(fix16_from_int(v)) {
  }

  fix16(float v) noexcept
    : value_(fix16_from_float(v)) {
  }

  fix16(double v) noexcept
    : value_(fix16_from_dbl(v)) {
  }

  fix16(const fix16& o) noexcept
    : value_(o.value_) {
  }

  fix16(fix16&& o) noexcept {
    std::swap(value_, o.value_);
  }

  explicit operator fix16_t(void) const noexcept {
    return value_;
  }

  explicit operator std::int16_t(void) const noexcept {
    return fix16_to_int(value_);
  }

  explicit operator float(void) const noexcept {
    return fix16_to_float(value_);
  }

  explicit operator double(void) const noexcept {
    return fix16_to_dbl(value_);
  }

  fix16& operator=(const fix16& r) noexcept {
    if (this != &r)
      value_ = r.value_;
    return *this;
  }

  fix16& operator=(fix16&& r) noexcept {
    if (this != &r)
      std::swap(value_, r.value_);
    return *this;
  }

  fix16& operator=(fix16_t r) noexcept {
    value_ = r;
    return *this;
  }

  fix16& operator=(std::int16_t r) noexcept {
    value_ = fix16_from_int(r);
    return *this;
  }

  fix16& operator=(float r) noexcept {
    value_ = fix16_from_float(r);
    return *this;
  }

  fix16& operator=(double r) noexcept {
    value_ = fix16_from_float(r);
    return *this;
  }

  fix16& operator+=(const fix16& r) noexcept {
    value_ += r.value_;
    return *this;
  }

  fix16& operator+=(fix16_t r) noexcept {
    value_ += r;
    return *this;
  }

  fix16& operator+=(std::int16_t r) noexcept {
    value_ += fix16_from_int(r);
    return *this;
  }

  fix16& operator+=(float r) noexcept {
    value_ += fix16_from_float(r);
    return *this;
  }

  fix16& operator+=(double r) noexcept {
    value_ += fix16_from_dbl(r);
    return *this;
  }

  fix16& operator-=(const fix16& r) noexcept {
    value_ -= r.value_;
    return *this;
  }

  fix16& operator-=(fix16_t r) noexcept {
    value_ -= r;
    return *this;
  }

  fix16& operator-=(std::int16_t r) noexcept {
    value_ -= fix16_from_int(r);
    return *this;
  }

  fix16& operator-=(float r) noexcept {
    value_ -= fix16_from_float(r);
    return *this;
  }

  fix16& operator-=(double r) noexcept {
    value_ -= fix16_from_dbl(r);
    return *this;
  }

  fix16& operator*=(const fix16& r) noexcept {
    value_ = fix16_mul(value_, r.value_);
    return *this;
  }

  fix16& operator*=(fix16_t r) noexcept {
    value_ = fix16_mul(value_, r);
    return *this;
  }

  fix16& operator*=(std::int16_t r) noexcept {
    value_ *= r;
    return *this;
  }

  fix16& operator*=(float r) noexcept {
    value_ = fix16_mul(value_, fix16_from_float(r));
    return *this;
  }

  fix16& operator*=(double r) noexcept {
    value_ = fix16_mul(value_, fix16_from_dbl(r));
    return *this;
  }

  fix16& operator/=(const fix16& r) noexcept {
    value_ = fix16_div(value_, r.value_);
    return *this;
  }

  fix16& operator/=(fix16_t r) noexcept {
    value_ = fix16_div(value_, r);
    return *this;
  }

  fix16& operator/=(std::int16_t r) noexcept {
    value_ /= r;
    return *this;
  }

  fix16& operator/=(float r) noexcept {
    value_ = fix16_div(value_, fix16_from_float(r));
    return *this;
  }

  fix16& operator/=(double r) noexcept {
    value_ = fix16_div(value_, fix16_from_dbl(r));
    return *this;
  }

  fix16 operator+(const fix16& x) const noexcept {
    auto r = *this;
    r += x;
    return r;
  }

  fix16 operator+(fix16_t x) const noexcept {
    auto r = *this;
    r += x;
    return r;
  }

  fix16 operator+(std::int16_t x) const noexcept {
    auto r = *this;
    r += x;
    return r;
  }

  fix16 operator+(float x) const noexcept {
    auto r = *this;
    r += x;
    return r;
  }

  fix16 operator+(double x) const noexcept {
    auto r = *this;
    r += x;
    return r;
  }

  fix16 operator-(const fix16& x) const noexcept {
    auto r = *this;
    r -= x;
    return r;
  }

  fix16 operator-(fix16_t x) const noexcept {
    auto r = *this;
    r -= x;
    return x;
  }

  fix16 operator-(std::int16_t x) const noexcept {
    auto r = *this;
    r -= x;
    return r;
  }

  fix16 operator-(float x) const noexcept {
    auto r = *this;
    r -= x;
    return r;
  }

  fix16 operator-(double x) const noexcept {
    auto r = *this;
    r -= x;
    return r;
  }

  fix16 operator*(const fix16& x) const noexcept {
    auto r = *this;
    r *= x;
    return r;
  }

  fix16 operator*(fix16_t x) const noexcept {
    auto r = *this;
    r *= x;
    return r;
  }

  fix16 operator*(std::int16_t x) const noexcept {
    auto r = *this;
    r *= x;
    return r;
  }

  fix16 operator*(float x) const noexcept {
    auto r = *this;
    r *= x;
    return r;
  }

  fix16 operator*(double x) const noexcept {
    auto r = *this;
    r *= x;
    return r;
  }

  fix16 operator/(const fix16& x) const noexcept {
    auto r = *this;
    r /= x;
    return r;
  }

  fix16 operator/(fix16_t x) const noexcept {
    auto r = *this;
    r /= x;
    return r;
  }

  fix16 operator/(std::int16_t x) const noexcept {
    auto r = *this;
    r /= x;
    return r;
  }

  fix16 operator/(float x) const noexcept {
    auto r = *this;
    r /= x;
    return r;
  }

  fix16 operator/(double x) const noexcept {
    auto r = *this;
    r /= x;
    return r;
  }

  bool operator==(const fix16& r) const noexcept {
    return value_ == r.value_;
  }

  bool operator==(fix16_t r) const noexcept {
    return value_ == r;
  }

  bool operator==(std::int16_t r) const noexcept {
    return value_ == fix16_from_int(r);
  }

  bool operator==(float r) const noexcept {
    return value_ == fix16_from_float(r);
  }

  bool operator==(double r) const noexcept {
    return value_ == fix16_from_dbl(r);
  }

  bool operator!=(const fix16& r) const noexcept {
    return value_ != r.value_;
  }

  bool operator!=(fix16_t r) const noexcept {
    return value_ != r;
  }

  bool operator!=(std::int16_t r) const noexcept {
    return value_ != fix16_from_int(r);
  }

  bool operator!=(float r) const noexcept {
    return value_ != fix16_from_float(r);
  }

  bool operator!=(double r) const noexcept {
    return value_ != fix16_from_dbl(r);
  }

  bool operator>(const fix16& r) const noexcept {
    return value_ > r.value_;
  }

  bool operator>(fix16_t r) const noexcept {
    return value_ > r;
  }

  bool operator>(std::int16_t r) const noexcept {
    return value_ > fix16_from_int(r);
  }

  bool operator>(float r) const noexcept {
    return value_ > fix16_from_float(r);
  }

  bool operator>(double r) const noexcept {
    return value_ > fix16_from_dbl(r);
  }

  bool operator>=(const fix16& r) const noexcept {
    return value_ >= r.value_;
  }

  bool operator>=(fix16_t r) const noexcept {
    return value_ >= r;
  }

  bool operator>=(std::int16_t r) const noexcept {
    return value_ >= fix16_from_int(r);
  }

  bool operator>=(float r) const noexcept {
    return value_ >= fix16_from_float(r);
  }

  bool operator>=(double r) const noexcept {
    return value_ >= fix16_from_dbl(r);
  }

  bool operator<(const fix16& r) const noexcept {
    return value_ < r.value_;
  }

  bool operator<(fix16_t r) const noexcept {
    return value_ < r;
  }

  bool operator<(std::int16_t r) const noexcept {
    return value_ < fix16_from_int(r);
  }

  bool operator<(float r) const noexcept {
    return value_ < fix16_from_float(r);
  }

  bool operator<(double r) const noexcept {
    return value_ < fix16_from_dbl(r);
  }

  bool operator<=(const fix16& r) const noexcept {
    return value_ <= r.value_;
  }

  bool operator<=(fix16_t r) const noexcept {
    return value_ <= r;
  }

  bool operator<=(std::int16_t r) const noexcept {
    return value_ <= fix16_from_int(r);
  }

  bool operator<=(float r) const noexcept {
    return value_ <= fix16_from_float(r);
  }

  bool operator<=(double r) const noexcept {
    return value_ <= fix16_from_dbl(r);
  }
};

}
