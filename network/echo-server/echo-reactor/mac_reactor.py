#!/usr/bin/env python
# -*- coding: UTF-8 -*-
#
# Copyright (c) 2016 ASMlover. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#  * Redistributions of source code must retain the above copyright
#    notice, this list ofconditions and the following disclaimer.
#
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materialsprovided with the
#    distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
import socket
import select

connections = {}
handlers = {}
def main():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_socket.bind(('0.0.0.0', 5555))
    server_socket.listen(5)

    kq = select.kqueue()

    kq.control([select.kevent(server_socket.fileno(), select.KQ_FILTER_READ, select.KQ_EV_ADD)], 0, 0)
    handlers[server_socket.fileno()] = handle_accept
    while True:
        events = kq.control([], 10000)
        for event in events:
            handler = handlers[event.ident]
            handler(kq, server_socket, event)

def handle_input(socket, data):
    socket.send(data)

def handle_request(kq, server_socket, event):
    if event.filter == select.KQ_FILTER_READ:
        client_socket = connections[event.ident]
        data = client_socket.recv(4096)
        if data:
            handle_input(client_socket, data)
        else:
            kq.control([select.kevent(event.ident, select.KQ_FILTER_READ, select.KQ_EV_DELETE)], 0, 0)
            client_socket.close()
            connections.pop(event.ident, None)
            handlers.pop(event.ident, None)

def handle_accept(kq, server_socket, event):
    (client_socket, client_address) = server_socket.accept()
    print 'got connection from', client_address
    kq.control([select.kevent(client_socket.fileno(), select.KQ_FILTER_READ, select.KQ_EV_ADD)], 0, 0)
    connections[client_socket.fileno()] = client_socket
    handlers[client_socket.fileno()] = handle_request

if __name__ == '__main__':
    main()
