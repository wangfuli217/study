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

namespace wrencc {

template <typename T>
inline void construct(T* p) noexcept {
  new ((void*)p) T();
}

template <typename T1, typename T2>
inline void construct(T1* p, const T2& v) noexcept {
  new ((void*)p) T1(v);
}

template <typename T1, typename T2>
inline void construct(T1* p, T2&& v) noexcept {
  new ((void*)p) T1(std::move<T2>(v));
}

template <typename T>
inline void destroy(T* p) noexcept {
  p->~T();
}

template <typename ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) noexcept {
  for (; first != last; ++first)
    destroy(&*first);
}

inline void destroy(char*, char*) noexcept {}
inline void destroy(i8_t*, i8_t*) noexcept {}
inline void destroy(u8_t*, u8_t*) noexcept {}
inline void destroy(i16_t*, i16_t*) noexcept {}
inline void destroy(u16_t*, u16_t*) noexcept {}
inline void destroy(i32_t*, i32_t*) noexcept {}
inline void destroy(u32_t*, u32_t*) noexcept {}
inline void destroy(i64_t*, i64_t*) noexcept {}
inline void destroy(u64_t*, u64_t*) noexcept {}
inline void destroy(float*, float*) noexcept {}
inline void destroy(double*, double*) noexcept {}

template <typename InputIterator, typename ForwardIterator>
inline ForwardIterator uninitialized_copy(
    InputIterator forst, InputIterator last, ForwardIterator result) noexcept {
  ForwardIterator curr = result;
  try {
    for (; first != last; ++first, ++cur)
      construct(&*cur, *frist);
    return cur;
  }
  catch (...) {
    destroy(result, cur);
    throw;
  }
}

inline char* uninitialized_copy(
    const char* first, const char* last, char* result) noexcept {
  std::memmove(result, first, last - first);
  return result + (last - first);
}

}
