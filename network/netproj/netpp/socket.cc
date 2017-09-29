// Copyright (c) 2017 ASMlover. All rights reserved.
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
#include "netpp_internal.h"
#include "utils.h"
#include "primitive.h"
#include "address.h"
#include "buffer.h"
#include "socket.h"

namespace netpp {

BaseSocket::BaseSocket(void) {
}

BaseSocket::~BaseSocket(void) {
}

void BaseSocket::open(int family, int socket_type, int protocol) {
  std::error_code ec;
  fd_ = socket::open(family, socket_type, protocol, ec);
  netpp::throw_error(ec, "open");
}

void BaseSocket::open(int family,
    int socket_type, int protocol, std::error_code& ec) {
  fd_ = socket::open(family, socket_type, protocol, ec);
}

void BaseSocket::close(void) {
  std::error_code ec;
  socket::close(fd_, ec);
  netpp::throw_error(ec, "close");
}

void BaseSocket::close(std::error_code& ec) {
  socket::close(fd_, ec);
}

void BaseSocket::shutdown(int what) {
  std::error_code ec;
  socket::shutdown(fd_, what, ec);
  netpp::throw_error(ec, "shutdown");
}

void BaseSocket::shutdown(int what, std::error_code& ec) {
  socket::shutdown(fd_, what, ec);
}

void BaseSocket::bind(const Address& addr) {
  std::error_code ec;
  socket::bind(fd_, addr.get_address(), ec);
  netpp::throw_error(ec, "bind");
}

void BaseSocket::bind(const Address& addr, std::error_code& ec) {
  socket::bind(fd_, addr.get_address(), ec);
}

void BaseSocket::connect(const Address& addr) {
  std::error_code ec;
  socket::connect(fd_, addr.get_address(), ec);
  netpp::throw_error(ec, "connect");
}

void BaseSocket::connect(const Address& addr, std::error_code& ec) {
  socket::connect(fd_, addr.get_address(), ec);
}

void BaseSocket::async_connect(
    const Address& addr, const ConnectHandler& handler) {
  // TODO: need io_service
}

void BaseSocket::async_connect(const Address& addr, ConnectHandler&& handler) {
  // TODO: need io_service
}

void BaseSocket::non_blocking(bool mode) {
  std::error_code ec;
  if (socket::set_non_blocking(fd_, mode, ec))
    non_blocking_ = mode;
  netpp::throw_error(ec, "non_blocking");
}

void BaseSocket::non_blocking(bool mode, std::error_code& ec) {
  if (socket::set_non_blocking(fd_, mode, ec))
    non_blocking_ = mode;
}

std::size_t TcpSocket::read(const MutableBuffer& buf) {
  std::error_code ec;
  auto nread = socket::read(get_fd(), buf.size(), buf.data(), ec);
  netpp::throw_error(ec, "read");
  return nread;
}

std::size_t TcpSocket::read(const MutableBuffer& buf, std::error_code& ec) {
  return socket::read(get_fd(), buf.size(), buf.data(), ec);
}

void TcpSocket::async_read(
    const MutableBuffer& buf, const ReadHandler& handler) {
  // TODO: need io_service
}

void TcpSocket::async_read(const MutableBuffer& buf, ReadHandler&& handler) {
  // TODO: need io_service
}

std::size_t TcpSocket::read_some(const MutableBuffer& buf) {
  std::error_code ec;
  auto nread = socket::read(get_fd(), buf.size(), buf.data(), ec);
  netpp::throw_error(ec, "read_some");
  return nread;
}

std::size_t TcpSocket::read_some(const MutableBuffer& buf, std::error_code& ec) {
  return socket::read(get_fd(), buf.size(), buf.data(), ec);
}

std::size_t TcpSocket::write(const ConstBuffer& buf) {
  std::error_code ec;
  auto nwrote = socket::write(get_fd(), buf.data(), buf.size(), ec);
  netpp::throw_error(ec, "write");
  return nwrote;
}

std::size_t TcpSocket::write(const ConstBuffer& buf, std::error_code& ec) {
  return socket::write(get_fd(), buf.data(), buf.size(), ec);
}

void TcpSocket::async_write(
    const ConstBuffer& buf, const WriteHandler& handler) {
  // TODO: need io_service
}

void TcpSocket::async_write(const ConstBuffer& buf, WriteHandler&& handler) {
  // TODO: need io_service
}

std::size_t TcpSocket::write_some(const ConstBuffer& buf) {
  std::error_code ec;
  auto nwrote = socket::write(get_fd(), buf.data(), buf.size(), ec);
  netpp::throw_error(ec, "write_some");
  return nwrote;
}

std::size_t TcpSocket::write_some(const ConstBuffer& buf, std::error_code& ec) {
  return socket::write(get_fd(), buf.data(), buf.size(), ec);
}

std::size_t UdpSocket::read(const MutableBuffer& buf) {
  std::error_code ec;
  auto nread = socket::read(get_fd(), buf.size(), buf.data(), ec);
  netpp::throw_error(ec, "read");
  return nread;
}

std::size_t UdpSocket::read(const MutableBuffer& buf, std::error_code& ec) {
  return socket::read(get_fd(), buf.size(), buf.data(), ec);
}

void UdpSocket::async_read(
    const MutableBuffer& buf, const ReadHandler& handler) {
  // TODO: need io_service
}

void UdpSocket::async_read(const MutableBuffer& buf, ReadHandler&& handler) {
  // TODO: need io_service
}

std::size_t UdpSocket::read_from(const MutableBuffer& buf, Address& addr) {
  std::error_code ec;
  auto nread = socket::read_from(get_fd(),
      buf.size(), buf.data(), addr.get_address(), ec);
  netpp::throw_error(ec, "read_from");
  return nread;
}

std::size_t UdpSocket::read_from(
    const MutableBuffer& buf, Address& addr, std::error_code& ec) {
  return socket::read_from(get_fd(),
      buf.size(), buf.data(), addr.get_address(), ec);
}

void UdpSocket::async_read_from(
    const MutableBuffer& buf, Address& addr, const ReadHandler& handler) {
  // TODO: need io_service
}

void UdpSocket::async_read_from(
    const MutableBuffer& buf, Address& addr, ReadHandler&& handler) {
  // TODO: need io_service
}

std::size_t UdpSocket::write(const ConstBuffer& buf) {
  std::error_code ec;
  auto nwrote = socket::write(get_fd(), buf.data(), buf.size(), ec);
  netpp::throw_error(ec, "write");
  return nwrote;
}

std::size_t UdpSocket::write(const ConstBuffer& buf, std::error_code& ec) {
  return socket::write(get_fd(), buf.data(), buf.size(), ec);
}

void UdpSocket::async_write(
    const ConstBuffer& buf, const WriteHandler& handler) {
  // TODO: need io_service
}

void UdpSocket::async_write(const ConstBuffer& buf, WriteHandler&& handler) {
  // TODO: need io_service
}

std::size_t UdpSocket::write_to(const ConstBuffer& buf, const Address& addr) {
  std::error_code ec;
  auto nwrote = socket::write_to(get_fd(),
      buf.data(), buf.size(), addr.get_address(), ec);
  netpp::throw_error(ec, "write_to");
  return nwrote;
}

std::size_t UdpSocket::write_to(
    const ConstBuffer& buf, const Address& addr, std::error_code& ec) {
  return socket::write_to(get_fd(),
      buf.data(), buf.size(), addr.get_address(), ec);
}

void UdpSocket::async_write_to(
    const ConstBuffer& buf, const Address& addr, const WriteHandler& handler) {
  // TODO: need io_service
}

void UdpSocket::async_write_to(
    const ConstBuffer& buf, const Address& addr, WriteHandler&& handler) {
  // TODO: need io_service
}

}
