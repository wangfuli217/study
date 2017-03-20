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

import random
import sys
import traceback
import async_timer as _at
from common.nyx_codec import CodecDecoder
from common.nyx_entity import EntityFactory, EntityManager
from common.nyx_id import IdCreator
from common import nyx_rpc_utils as _rpcutils
from distributed.game import nyx_game_global as _gglobal
from proto.pyproto import nyx_common_pb2 as _c_pb2
from proto.pyproto import nyx_gate_game_pb2 as _gg_pb2
from proto.pyproto import nyx_game_manager_pb2 as _gm_pb2
from log.nyx_log import LogManager
from server.nyx_client_proxy import ClientProxy
from server.nyx_server_entity import AvatarEntity, PrepareEntity, PostEntity

class GameClientProxy(object):
    def __init__(self, stub):
        super(GameClientProxy, self).__init__()
        # TODO:
        pass

    def global_entity_message(self, global_entity_msg):
        # TODO:
        pass

    def forward_entity_message(self, entity_msg):
        # TODO:
        pass

    def create_real_entity(self, create_info):
        # TODO:
        pass

    def real_entity_created(self, entity_mailbox):
        # TODO:
        pass

    def bind_client_to_game(self, bindmsg):
        # TODO:
        pass

class GameClientProxyManager(object):
    def __init__(self):
        super(GameClientProxyManager, self).__init__()
        # TODO:
        pass

    def add_gate_proxy(self, proxy, gate_id=None):
        # TODO:
        pass

    def del_gate_proxy(self, gate_id):
        # TODO:
        pass

    def get_random_gate_proxy(self):
        # TODO:
        pass

    def has_gate_proxy(self, gate_id):
        # TODO:
        pass

    def get_gate_proxies(self):
        # TODO:
        pass
