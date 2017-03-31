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

import cPickle
import collections
import async_timer as _at
from common.nyx_log import LogManager
from common.nyx_codec import CodecDecoder
from common.nyx_entity import EntityFactory
from common.nyx_entity import EntityManager
from common.nyx_id import IdCreator
from common import nyx_rpc_utils as _rpcutils
from common import nyx_common as _nc
from distributed.game import nyx_game_global as _gglobal
from distributed.game import nyx_game_utils as _gutils
from log.nyx_log import LogManager
from proto.pyproto import nyx_common_pb2 as _c_pb2
from proto.pyproto import nyx_gate_game_pb2 as _gg_pb2
from proto.pyproto import nyx_game_manager_pb2 as _gm_pb2
from proto.pyproto.nyx_common_pb2 import EntityMailbox
from rpc.nyx_channel_client import ChannelClient
from server.nyx_client_proxy import ClientProxy
from server.nyx_server_entity import AvatarEntity
from server.nyx_server_entity import PrepareEntity
from server.nyx_server_entity import PostEntity

class GameManagerProxy(object):
    """game和game manager通信的rpc client代理"""
    def __init__(self, stub):
        super(GameManagerProxy, self).__init__()
        self.logger = LogManager.get_logger('NyxCore.Game.GameManagerProxy')
        # TODO:
        pass

    def on_channel_disconnected(self, rpc_channel):
        # TODO:
        pass

    def reg_game_server(self, server_info):
        # TODO:
        pass

    def reg_global_entity_mailbox(self, entity_regmsg):
        # TODO:
        pass

    def unreg_global_entity_mailbox(self, entity_regmsg):
        # TODO:
        pass

    def forward_entity_message(self, entity_msg):
        # TODO:
        pass

    def global_entity_message(self, entity_msg):
        # TODO:
        pass

    def global_message(self, global_msg):
        # TODO:
        pass

    def global_data(self, global_data):
        # TODO:
        pass

    def del_global_data(self, global_data):
        # TODO:
        pass

    def game_callback(self, callback_type, callback_id, return_value):
        # TODO:
        pass

    def create_entity(self, entity_info):
        # TODO:
        pass

class GameManagerClient(_gm_pb2.SGameManagerToClient):
    _STATUS_NOT_CONNECTED = 1
    _STATUS_CONNECTING = 2
    _STATUS_CONNECTED = 3

    def __init__(self, config, config_sections):
        # TODO:
        pass

    def start_connect(self, timeout):
        """开始连接game manager"""
        # TODO:
        pass

    def channel_callback(self, rpc_channel):
        # TODO:
        pass

    def on_channel_disconnected(self, rpc_channel):
        # TODO:
        pass

    def on_reg_global_entity_mailbox(self, controller, entity_regmsg, done):
        """注册global mailbox的rpc回调"""
        # TODO:
        pass

    def on_unreg_global_entity_mailbox(self, controller, entity_regmsg, done):
        """反注册global mailbox的rpc回调"""
        # TODO:
        pass

    def _get_rpc_method(self, entity, method):
        """查找entity的rpc方法"""
        # TODO:
        pass

    def on_forward_entity_message(self, controller, entity_msg, done):
        """game manager收到转发过来的msg"""
        # TODO:
        pass

    def on_global_entity_message(self, controller, global_entity_msg, done):
        # TODO:
        pass

    def on_global_message(self, controller, global_msg, done):
        """game manager转发过来的全局消息"""
        # TODO:
        pass

    def on_global_data(self, controller, global_data, done):
        """game manager转发过来的全局数据信息"""
        # TODO:
        pass

    def on_del_global_data(self, controller, global_data, done):
        """game manager转发过来的删除全局数据消息"""
        # TODO:
        pass

    def _create_entity(self, entity_type, entity_info):
        # TODO:
        pass

    def create_entity(self, controller, entity_info, done):
        """game manager通知创建entity"""
        # TODO:
        pass

    def post_create_entity(self, entity, entity_info_header):
        # TODO:
        pass

    def game_manager_callback(self, controller, return_value, done):
        # TODO:
        pass

    def on_get_game_server_list(self, controller, game_server_infos, done):
        # TODO:
        pass

    def run_script(self, controller, script, done):
        # TODO:
        pass

    def on_server_control(self, controller, script, done):
        # TODO:
        pass

    def _notify_server_closing(self):
        """通知服务器将要关闭"""
        # TODO:
        pass

    def _notify_server_closed(self):
        """通知服务器关闭"""
        # TODO:
        pass

    def _server_closed_callback(self, status):
        # TODO:
        pass

    def _get_alive_entities_count_as_dict(self):
        # TODO:
        pass

    def _check_saved_success(self):
        # TODO:
        pass