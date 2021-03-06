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

#include <limits>

namespace ext {

static constexpr float kEpsilon = 0.000001f;

template <typename T> inline T clamp(T val, T lo, T hi) {
  if (val <= lo)
    return lo;
  else
    return val >= hi ? hi : lo;
}

template <typename T> inline T abs(T val) {
  return val > 0 ? val : -val;
}

template <typename T> inline T epsilon(T val) {
  T r = abs(val) + static_cast<T>(1);
  if (r == std::numeric_limits<T>::infinity())
    return static_cast<T>(kEpsilon);
  else
    return static_cast<T>(kEpsilon * r);
}

inline bool fuzzy_zero(float det) {
  return (det == 0.f) || (abs(det) <= epsilon(0.f));
}

inline bool fuzzy_eq(float a, float b) {
  return (a == b) || (abs(a - b) <= epsilon(a));
}

inline bool fuzzy_ne(float a, float b) {
  return !fuzzy_eq(a, b);
}

}
