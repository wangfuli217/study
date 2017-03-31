#!/usr/bin/env python
# -*- coding: UTF-8 -*-
#
# Copyright (c) 2017 ASMlover. All rights reserved.
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

import async_timer as _at
from distributed.gate import nyx_gate_global as _gglobal
from distributed.gate.nyx_game_client import GameServerClient
from log.nyx_log import LogManager
from proto.pyproto import nyx_common_pb2 as _c_pb2
from proto.pyproto import nyx_game_manager_pb2 as _gm_pb2
from rpc.nyx_channel_client import ChannelClient

class GameServerFinder(_gm_pb2.SGameManagerToClient):
    def __init__(self, addr, port, proxy_mgr, gate_id, config, config_sections):
        super(GameServerFinder, self).__init__()
        self.logger = LogManager.get_logger('NyxCore.Gate.GameServerFinder')
        # TODO:
        pass

    def start_connect(self, timeout):
        # TODO:
        pass

    def channel_callback(self, rpc_channel):
        # TODO:
        pass

    def get_game_server_list(self):
        # TODO:
        pass

    def on_channel_disconnected(self, rpc_channel):
        # TODO:
        pass

    def _move_operation(self, key, src_set, dst_set,
            ban_client=False, server_type=_c_pb2.ServerInfo.NORMAL):
        # TODO:
        pass

    def connect_game_server(self):
        # TODO:
        pass

    def is_all_connected(self):
        # TODO:
        pass

    def on_get_game_server_list(self, controller, server_list, done):
        # TODO:
        pass

    def on_server_control(self, controller, control_type, done):
        # TODO:
        pass