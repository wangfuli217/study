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
#include <iostream>
#include <thread>
#include "../core/Nyx.hh"

void run_client(void) {
  bool running{true};
  auto client = nyx::make_new_client();
  client->set_resolve_error_callback([](const nyx::SessionPtr& conn) {
        std::cout << "client: resolve connect endpoint failed ..." << std::endl;
      }).set_connected_callback([](const nyx::SessionPtr& conn) {
        std::cout << "client: connect to server success ..." << std::endl;
      }).set_connect_error_callback([](const nyx::SessionPtr& conn) {
        std::cout << "client: connect to server failed ..." << std::endl;
      }).set_message_callback(
        [](const nyx::SessionPtr& conn, const std::string& buf) {
        std::cout << "client: recv message from server: " << buf << std::endl;
      }).set_disconnected_callback([&running](const nyx::SessionPtr& conn) {
        std::cout << "client: on disconnected ..." << std::endl;
        running = false;
      });
  client->async_connect("127.0.0.1", 5555);

  std::thread t([&running] {
        nyx::launch();
        while (running)
          nyx::poll();
        nyx::shutoff();
      });

  std::string line;
  while (running && std::getline(std::cin, line)) {
    if (line == "exit") {
      client->disconnect();
      running = false;
      break;
    }

    client->async_write(line);
    line.clear();
  }
  t.join();
}
