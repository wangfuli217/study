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
#include <fstream>
#include <iostream>
#include <sstream>
#include "compiler.hh"
#include "vm.hh"

static std::string _s_root_dir;

static std::string read_module(wrencc::WrenVM& vm, const std::string& module) {
  std::string path = _s_root_dir + module + ".wren";
  std::ifstream fp(path);
  if (fp.is_open()) {
    std::stringstream ss;
    ss << fp.rdbuf();

    return ss.str();
  }
  return "";
}

static void eval_with_file(const std::string& fname) {
  auto pos = fname.find_last_of('\\');
  if (pos != std::string::npos)
    _s_root_dir = fname.substr(0, pos + 1);

  std::ifstream fp(fname);
  if (fp.is_open()) {
    std::stringstream ss;
    ss << fp.rdbuf();

    wrencc::WrenVM vm;
    vm.set_load_fn(read_module);
    vm.interpret(fname, ss.str());
  }
}

int main(int argc, char* argv[]) {
  (void)argc, (void)argv;

  std::cout << "W R E N - C C" << std::endl;

  eval_with_file(argv[1]);

  return 0;
}
