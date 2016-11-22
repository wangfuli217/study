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
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <vector>
#include <chaos/Platform.h>
#include <chaos/Types.h>
#include <chaos/error/SystemError.h>
#include <chaos/datetime/Date.h>
#include <chaos/datetime/Timezone.h>

namespace chaos {

struct Transition {
  time_t gmttime;
  time_t localtime;
  int localtime_index;

  Transition(time_t gmt, time_t lt, int index)
    : gmttime(gmt)
    , localtime(lt)
    , localtime_index(index) {
  }
};

struct TransitionCompare {
  bool compare_with_gmt{};

  explicit TransitionCompare(bool cmp_with_gmt)
    : compare_with_gmt(cmp_with_gmt) {
  }

  bool operator()(const Transition& a, const Transition& b) const {
    if (compare_with_gmt)
      return a.gmttime < b.gmttime;
    else
      return a.localtime < b.localtime;
  }

  bool equal(const Transition& a, const Transition& b) const {
    if (compare_with_gmt)
      return a.gmttime == b.gmttime;
    else
      return a.localtime == b.localtime;
  }
};

struct Localtime {
  time_t gmtoff;
  bool isdst;
  int arrb_index;

  Localtime(time_t _gmtoff, bool _isdst, int _arrb)
    : gmtoff(_gmtoff)
    , isdst(_isdst)
    , arrb_index(_arrb) {
  }
};

static const int kSecondsPerDay = 86400; // 24 * 60 * 60

struct TZData {
  std::vector<Transition> transitions;
  std::vector<Localtime> localtimes;
  std::vector<std::string> names;
  std::string abbreviation;
};

inline void fill_time(uint32_t sec, struct tm& utc) {
  uint32_t min = sec / 60;
  utc.tm_hour = min / 60;
  utc.tm_min = min % 60;
  utc.tm_sec = sec % 60;
}

class TZFile : private UnCopyable {
  FILE* stream_{};
public:
  TZFile(const char* fname)
    : stream_(fopen(fname, "rb")) {
  }

  ~TZFile(void) {
    if (nullptr != stream_)
      fclose(stream_);
  }

  bool is_valid(void) const {
    return nullptr != stream_;
  }

  std::string read_bytes(size_t bytes) {
    CHAOS_ARRAY(char, buf, bytes);
    size_t n = fread(buf, 1, bytes, stream_);
    if (n != bytes)
      __chaos_throw_exception(std::logic_error("no enough data"));
    return buf;
  }

  template <typename Integer>
  Integer read_integer(void) {
    static_assert(
        (sizeof(Integer) == sizeof(int8_t) ||
         sizeof(Integer) == sizeof(int16_t) ||
         sizeof(Integer) == sizeof(int32_t) ||
         sizeof(Integer) == sizeof(int64_t)),
        "Integer size should be `1`, `2`, `4`, `8` bytes");

    Integer x = 0;
    size_t n = fread(&x, 1, sizeof(Integer), stream_);
    if (n != sizeof(Integer))
      __chaos_throw_exception(std::logic_error("bad Integer data"));
    return x;
  }

  uint8_t read_uint8(void) {
    return read_integer<uint8_t>();
  }

  int32_t read_int32(void) {
    return read_integer<int32_t>();
  }
};

}
