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
#include <thread>
#include <vector>
#include <core/BaseServer.h>
#include <core/ServerManager.h>

namespace nyx {

using ThreadPtr = std::shared_ptr<std::thread>;

ServerManager::ServerManager(void)
  : nthreads_(kDefaultThreads) {
}

ServerManager::~ServerManager(void) {
}

void ServerManager::add_server(const ServerPtr& s) {
  std::unique_lock<std::mutex> guard(mutex_);
  servers_.insert(s);
}

void ServerManager::start(void) {
  std::vector<ThreadPtr> threads;
  for (auto i = 0u; i < nthreads_; ++i)
    threads.emplace_back(new std::thread([this] { context_.run(); }));

  for (auto& t : threads)
    t->join();
}

void ServerManager::stop(void) {
  std::unique_lock<std::mutex> guard(mutex_);
  for (auto& s : servers_)
    s->invoke_shutoff();
  servers_.clear();
}

void ServerManager::set_worker(void) {
  std::unique_lock<std::mutex> guard(mutex_);
  worker_.reset(new asio::io_context::work(context_));
}

void ServerManager::unset_worker(void) {
  std::unique_lock<std::mutex> guard(mutex_);
  worker_.reset();
}

}