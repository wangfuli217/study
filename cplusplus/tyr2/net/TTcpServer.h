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
#ifndef __TYR_NET_TCPSERVER_HEADER_H__
#define __TYR_NET_TCPSERVER_HEADER_H__

#include <atomic>
#include <map>
#include <memory>
#include <string>
#include "../basic/TTypes.h"
#include "TCallbacks.h"

namespace tyr { namespace net {

class Acceptor;
class EventLoop;
class EventLoopThreadPool;
class InetAddress;

class TcpServer : private basic::UnCopyable {
  typedef std::function<void (EventLoop*)> ThreadInitCallback;
  typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

  int next_connid_{1};
  std::atomic<int32_t> started_{0};
  EventLoop* loop_{}; // acceptor event loop
  const std::string ip_port_;
  const std::string name_;
  std::unique_ptr<Acceptor> acceptor_;
  std::shared_ptr<EventLoopThreadPool> thread_pool_;
  ConnectionCallback connection_fn_{};
  MessageCallback message_fn_{};
  WriteCompleteCallback write_complete_fn_{};
  ThreadInitCallback thread_init_fn_{};
  ConnectionMap connections_;

  void new_connection(int sockfd, const InetAddress& peeraddr);
  void remove_connection(const TcpConnectionPtr& conn);
  void remove_connection_in_loop(const TcpConnectionPtr& conn);
public:
  enum Option {
    OPTION_UNREUSEPORT,
    OPTION_REUSEPORT,
  };
  TcpServer(EventLoop* loop, const InetAddress& host_addr, const std::string& name, Option opt = OPTION_UNREUSEPORT);
  ~TcpServer(void);

  void start(void);
  void set_thread_count(int thread_count);

  const std::string& get_ip_port(void) const {
    return ip_port_;
  }

  const std::string& get_name(void) const {
    return name_;
  }

  EventLoop* get_loop(void) const {
    return loop_;
  }

  std::shared_ptr<EventLoopThreadPool> get_thread_pool(void) {
    return thread_pool_;
  }

  void set_thread_init_callback(const ThreadInitCallback& fn) {
    thread_init_fn_ = fn;
  }

  void set_connection_callback(const ConnectionCallback& fn) {
    connection_fn_ = fn;
  }

  void set_message_callback(const MessageCallback& fn) {
    message_fn_ = fn;
  }

  void set_write_complete_callback(const WriteCompleteCallback& fn) {
    write_complete_fn_ = fn;
  }
};

}}

#endif // __TYR_NET_TCPSERVER_HEADER_H__
