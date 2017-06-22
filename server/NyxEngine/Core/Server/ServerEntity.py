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

import async_timer
from Common.Codec import Md5Cache
from Common.EntityUtils import EntityFactory, EntityManager
from Common.IdUtils import IdUtils
from Distributed.Game import GameAPI
from Distributed.Game import GameGlobal
from Log.LogManager import LogManager
from Proto.Details import BasicProtocol_pb2 as _B_PB2
from Proto.Details import Common_pb2 as _C_PB2
from Server.ClientProxy import ClientProxy

class DirtyManager(object):
    _dirties = {}

    @staticmethod
    def add_dirty_state(id, is_dirty):
        DirtyManager._dirties[id] = is_dirty

    @staticmethod
    def del_dirty_state(id):
        DirtyManager._dirties.pop(id, None)

    @staticmethod
    def set_dirty_state(id, is_dirty):
        if id in DirtyManager._dirties:
            DirtyManager._dirties[id] = is_dirty

    @staticmethod
    def get_dirty_state(id):
        return DirtyManager._dirties.get(id, False)

class EntityProxy(object):
    """服务端Entity的代理"""
    def __init__(self, entity, mailbox):
        self.entity = entity
        self.mailbox = mailbox

    def __getattr__(self, name):
        def _caller(*args):
            if args:
                parameters = {'_': args}
            else:
                parameters = None
            self.entity.call_server_method(self.mailbox, name, parameters)
        return _caller

class ServerEntity(object):
    """所有服务器对象的基类"""
    def __init__(self, entity_id=None):
        super(ServerEntity, self).__init__()
        self.logger = LogManager.get_logger(
                'ServerEntity.%s' % self.__class__.__name__)
        if entity_id is None:
            self.entity_id = IdUtils.gen_id()
        else:
            self.entity_id = entity_id
        self.gate_proxy = None
        self.save_timer = None
        self.is_destroyed = False
        save_interval = self.get_persistent_time()
        if save_interval > 0:
            self.save_timer = async_timer.add_cycle_timer(
                    save_interval, lambda: self.save())
        EntityManager.add_entity(self.entity_id, self, False)
        DirtyManager.add_dirty_state(self, False)

    def __getattribute__(self, key):
        if key != '__class__':
            DirtyManager.set_dirty_state(self, True)
        return super(ServerEntity, self).__getattribute__(key)

    def tick(self):
        """每Game Tick的时间调用一次"""
        pass

    def save(self, callback=None):
        if self.is_persistent():
            if DirtyManager.get_dirty_state(self):
                GameAPI.save_entity(self, callback)
                DirtyManager.set_dirty_state(self, False)
        elif callback:
            callback(True)

    def cancel_save_timer(self):
        if self.save_timer:
            self.save_timer.cancel()
            self.save_timer = None

    def _destroy_callback(self, r, callback):
        callback(r)
        EntityManager.del_entity(self.entity_id)

    def destroy(self, callback=None):
        """销毁当前Entity"""
        if self.is_destroyed:
            return
        self.cancel_save_timer()
        if callback is None:
            self.save()
            EntityManager.del_entity(self.entity_id)
        else:
            self.save(lambda r: self._destroy_callback(r, callback))
        DirtyManager.del_dirty_state(self)
        self.is_destroyed = True

    def init_from_dict(self, data_dict):
        """使用data_dict来初始化，只有序列化到持久存储的才需要"""
        pass

    def is_persistent(self):
        """表示该Entity是否持久化到DB"""
        return False

    def get_persistent_time(self):
        """返回存盘时间，默认时间为15s"""
        return GameGlobal.def_savetime

    def get_persistent_dict(self):
        """将自己序列化到一个dict，需要兼容BSON"""
        return {}

    def _get_client_info(self):
        return None

    def _get_gate_proxy(self):
        """得到Entity对应的GateProxy，Entity应该始终对应同一个GateProxy"""
        gate_id = self.gate_proxy.gate_id
        if self.gate_proxy is None:
            self.gate_proxy = GameGlobal.game_client_mgr.get_random_gate_proxy()
        elif not GameGlobal.game_client_mgr.has_gate_proxy(gate_id):
            self.gate_proxy = GameGlobal.game_client_mgr.get_random_gate_proxy()
            self.logger.warn(
                    'ServerEntity._get_gate_proxy: lost connection to Gate')
        return self.gate_proxy

    def call_server_method(self,
            mailbox, method, parameters=None, callback=None):
        """调用其他Game上的Entity方法，如果有Gate到Game的连接，则直接消息
        发送过去，否则通过GameManager将消息发送出去
        """
        msg = _C_PB2.EntityMessage()
        msg.entity_id = mailbox.entity_id
        msg.method.md5 = Md5Cache.get_md5(method)
        if parameters is not None:
            msg.parameters = str(GameGlobal.proto_encoder.encode(parameters))
        header = _B_PB2.ForwardMessageHeader()
        header.src_mailbox.entity_id = self.entity_id
        header.src_mailbox.server_info.CopyFrom(GameGlobal.game_info)
        header.dst_mailbox.CopyFrom(mailbox)
        gate_proxy = self._get_gate_proxy()
        if gate_proxy:
            # 找到了mailbox对应的porxy，直接转发消息
            if callback is not None:
                header.callback_id = GameGlobal.reg_gate_callback(callback)
            msg.routes = header.SerializeToString()
            gate_proxy.forward_entity_message(msg)
        else:
            # 没找到mailbox对应的proxy，通过Game Manager转发
            self.logger.info(
                    'ServerEntity.call_server_method: forward by game manager')
            if callback is not None:
                header.callback_id = GameGlobal.reg_gamemgr_callback(callback)
            msg.routes = header.SerializeToString()
            GameGlobal.gamemgr_proxy.forward_entity_message(msg)

    def transfer_to_server(self, dst_server, content=None, callback=None):
        """将某Entity转移到其他服务器

        * 在dst_server创建PreEntity，检查目标服务器能否迁移Entity
        * 将Entity迁移到dst_server，如果是Persistent的，且content不为None，需要
          经过数据库进行数据迁移
        * 在src_server创建PostEntity，转发S2S的RPC
        """
        if GameAPI.is_local_server(dst_server):
            self.logger.warn('ServerEntity.transfer_to_server: same as current')
            return

        if not self.is_persistent() and not content:
            content = self.get_persistent_dict()

        pre_entity_id = IdUtils.get_id()
        src_server = _C_PB2.ServerInfo()
        src_server.CopyFrom(GameGlobal.game_info)
        GameAPI._create_remote_entity('PreEntity', dst_server,
                entity_id=pre_entity_id, entity_content=None,
                fromdb=False, transfer_entity=True,
                callback=lambda x: self._on_create_pre_entity(
                    x, dst_server, pre_entity_id, content, callback))

    def _on_transfer_result(self, status, callback):
        if callback:
            try:
                callback(status)
            except:
                self.logger._log_exception()

    def _on_create_pre_entity(self,
            status, dst_server, pre_entity_id, content, callback):
        """创建PreEntity的回调"""
        if not status:
            self.logger.warn(
                    'ServerEntity._on_create_pre_entity: create PreEntity fail')
            self._on_transfer_result(status, callback)
        else:
            self._do_transfer_to_server(dst_server,
                    pre_entity_id, content, callback)
            # 保存并销毁自己，为了保证同一时刻只存在一个Entity
            self.destroy()
            self._create_post_entity(dst_server)

    def _do_transfer_to_server(self,
            dst_server, pre_entity_id, content, callback):
        """执行迁移工作"""
        gate_proxy = self._get_gate_proxy()
        if not gate_proxy:
            self.logger.error(
                    'ServerEntity._do_transfer_to_server: no gate proxy found')
            self._on_transfer_result(False, callback)
            return

        # 通知迁移成功
        self._on_transfer_result(True, callback)

        # 通知远端的PreEntity创建真的Entity
        info = _B_PB2.RealEntityCreateInfo()
        info.dst_server.CopyFrom(dst_server)
        info.src_server.CopyFrom(GameGlobal.game_info)
        info.migrate_entity_id = pre_entity_id
        info.real_entity_id = self.entity_id
        info.real_entity_type = self.__class__.__name__
        if content:
            info.content = GameGlobal.proto_encoder.encode(content)
        client_info = self._get_client_info()
        if client_info:
            info.client_info_cache = bytes(client_info.SerializeToString())
        gate_proxy.create_real_entity(info)

    def _create_post_entity(self, dst_server, bind_msg=None, client=None):
        """创建一个与自己entity_id一样的PostEntity，负责转发RPC"""
        entity = EntityFactory.get_instance().create_entity(
                'PostEntity', self.entity_id)
        entity.set_dest_server(dst_server)
        entity.set_bind_client_msg(bind_msg)
        entity.set_bind_client(client)

    def on_server_closing(self):
        """通知服务器即将关闭"""
        pass

    def on_server_closed(self, callback=None):
        """通知服务器关闭"""
        self.destroy(callback)

    def can_be_reconnected(self, auth_msg):
        """是否可以进行重连"""
        return self.client is not None

    def on_reconnected(self, auth_msg=None):
        """重新连接成功"""
        pass

    def get_reconnected_extra_msg(self):
        """获取重连时额外的数据信息"""
        return {}

    def copy_gate_proxy_to(self, other):
        """将自己的gate_proxy拷贝给另一个Entity"""
        other.gate_proxy = self.gate_proxy

    def get_mailbox(self):
        mailbox = _C_PB2.EntityMailbox()
        mailbox.entity_id = self.entity_id
        mailbox.server_info.CopyFrom(GameGlobal.game_info)
        return mailbox

class PreEntity(ServerEntity):
    """Entity迁移时，在目标服务器建立的Entity，生命期结束后自动销毁"""
    _LIFE_TIME = 20
    def __init__(self, entity_id=None):
        super(PreEntity, self).__init__(entity_id)
        self._destroy_handler = async_timer.add_timer(
                self._LIFE_TIME, lambda: self._on_destroy_handler)

    def _on_destroy_handler(self):
        self._destroy_handler = None
        self.destroy()

    def destroy(self, callback=None):
        if self._destroy_handler:
            self._destroy_handler.cancel()
            self._destroy_handler = None
        super(PreEntity, self).destroy(callback)

    def create_real_entity(self, create_info):
        real_entity_id = create_info.real_entity_id
        real_entity_type = create_info.real_entity_type
        real_entity = EntityManager.get_entity(real_entity_id)
        if real_entity:
            if isinstance(real_entity, PostEntity):
                # 如果有PostEntity，则销毁real_entity
                real_entity.destroy()
            else:
                self.destroy()
                return

        real_entity = None
        gate_proxy = self._get_gate_proxy()
        if not gate_proxy:
            self.logger.error(
                    'PreEntity.create_real_entity: cannot get gate proxy')
            return

        if create_info.HasField('content'):
            # 如果content中有数据则直接创建，否则从DB中读取数据
            content = GameGlobal.proto_encoder.decode(create_info.content)
            real_entity = EntityFactory.get_instance().create_entity(
                    real_entity_type, real_entity_id)
            if real_entity is None:
                # 创建Entity失败
                self.logger.error(
                        'PreEntity.create_real_entity: create real entity fail')
                self.destroy()
            else:
                real_entity.init_from_dict(content)
                self._create_entity_client(real_entity, create_info)
                mailbox = _C_PB2.EntityMailbox()
                mailbox.server_info.CopyFrom(create_info.src_server)
                mailbox.entity_id = real_entity_id
                gate_proxy.create_real_entity_successed(mailbox)
                self.destroy()
        else:
            r = GameAPI.create_entity_fromdb(real_entity_type, real_entity_id,
                    lambda x: self._on_create_entity_fromdb(x, create_info))
            if not r:
                self.logger.error(
                        'PreEntity.create_real_entity: create from db failed')
                self.destroy()

    def _create_entity_client(self, entity, create_info):
        """直接创建一个Entity客户端"""
        if create_info.HasField('client_info_cache'):
            client_info = _B_PB2.ClientInfo()
            client_info.ParseFromString(create_info.client_info_cache)
            client_proxy = ClientProxy(None, client_info)
            entity.set_client(client_proxy)

    def _on_create_entity_fromdb(self, entity, create_info):
        if not entity:
            self.logger.error(
                    'PreEntity._on_create_entity_fromdb: create from db failed')
            self.destroy()
            return

        self._create_entity_client(entity, create_info)
        mailbox = _C_PB2.EntityMailbox()
        mailbox.server_info.CopyFrom(create_info.src_server)
        mailbox.entity_id = entity.entity_id
        gate_proxy = self._get_gate_proxy()
        if not gate_proxy:
            self.logger.error(
                    'PreEntity._on_create_entity_fromdb: find gate proxy fail')
            self.destroy()
            return
        gate_proxy.create_real_entity_successed(mailbox)
        self.destroy()

class PostEntity(ServerEntity):
    """迁移时留在源服务器做RPC转发的Entity

    entity_id与目标服务器上真的Entity的entity_id一样，在接收到目标
    服务器上的Entity创建完成的通知之前，cache住转发的RPC
    """
    _MAX_CACHE_MESSAGE = 2000
    _LIFE_TIME = 20
    def __init__(self, entity_id=None):
        super(PostEntity, self).__init__(entity_id)
        self.dest_server = None
        self.bind_client = None # 绑定的客户端
        self.bind_client_msg = None
        self.cache_messages = []
        self.real_entity_created = False
        self._destroy_handler = async_timer.add_timer(
                self._LIFE_TIME, self._on_destroy_handler)

    def _on_destroy_handler(self):
        self._destroy_handler = None
        self.destroy()

    def destroy(self, callback=None):
        if self._destroy_handler:
            self._destroy_handler.cancel()
            self._destroy_handler = None
        if self.bind_client:
            self.bind_client.destroy()
            self.bind_client = None
        # 将cache的消息转发出去
        self._forward_all_messages()
        super(PostEntity, self).destroy(callback)

    def set_dest_server(self, dest_server):
        self.dest_server = dest_server

    def set_bind_client(self, client):
        self.bind_client = client

    def set_bind_client_msg(self, bind_msg):
        self.bind_client_msg = bind_msg

    def forward_message(self, entity_msg):
        """消息转发"""
        if self.real_entity_created:
            self._do_forward_message(entity_msg)
        else:
            self.cache_messages.append(entity_msg)
            if len(self.cache_messages) > self._MAX_CACHE_MESSAGE:
                # 缓存的消息过多，FIXME: 需要更好的处理
                self.cache_messages = []
                self.destroy()

    def _do_forward_message(self, entity_msg):
        header = _B_PB2.ForwardMessageHeader()
        header.ParseFromString(entity_msg.routes)
        header.dst_mailbox.server_info.CopyFrom(self.dest_server)
        entity_msg.routes = header.SerializeToString()

        gate_proxy = self._get_gate_proxy()
        if not gate_proxy:
            self.logger.error(
                    'PostEntity._do_forward_message: find gate proxy fail')
            return
        gate_proxy.forward_entity_message(entity_msg)

    def create_real_entity_successed(self):
        if self.bind_client:
            if self.bind_client_msg:
                self.bind_client.bind_client_to_game(self.bind_client_msg)
        self._forward_all_messages()
        self.real_entity_created = True

    def _forward_all_messages(self):
        for msg in self.cache_messages:
            self._do_forward_message(msg)
        self.cache_messages = []

class AvatarEntity(ServerEntity):
    """玩家控制的Entity，是与客户端Entity相对应"""
    def __init__(self, entity_id=None):
        super(AvatarEntity, self).__init__(entity_id)
        self.client = None

    def set_client(self, client):
        """绑定ClientProxy对象，以便与客户端Avatar互相通信"""
        if self.client is not None:
            self.client.set_owner(None)
            self.client.notify_disconnect_client()
        self.client = client
        if self.client is not None:
            self.client.set_owner(self)

    def destroy_client(self):
        """销毁用ClientProxy对象绑定的对象，不触发notify_disconnect_client"""
        if self.client is None:
            return
        self.client.destroy()
        self.client = None

    def on_transfered(self):
        """在Entity迁移到新Game上时在on_become_player之前调用"""
        pass

    def on_become_player(self):
        """Entity成为玩家可控制的Entity时调用"""
        self.client.create_entity(self.__class__.__name__,
                self.get_persistent_dict(), self.entity_id)
        self.client.call_client_method('become_player', {})

    def gave_client_to(self, other):
        """将ClientProxy移交给其他Entity"""
        other.set_client(self.client)
        other.on_become_player()
        self.client = None

    def destroy(self, callback=None):
        """销毁AvatarEntity"""
        if self.client is not None:
            self.client.destroy()
            self.client = None
        super(AvatarEntity, self).destroy(callback)

    def on_lose_client(self):
        """客户端断开连接的回调"""
        self.destroy()

    def on_client_message(self, method, entity_id, reliable):
        """有来自对应客户端的RPC消息"""
        pass

    def _on_create_pre_entity(self,
            status, dst_server, pre_entity_id, content, callback):
        if not status:
            self._on_transfer_result(status, callback)
        else:
            self._cache_client_message(dst_server,
                    pre_entity_id, content, callback)

    def _cache_client_message(self,
            dst_server, pre_entity_id, content, callback):
        """缓存客户端发送的RPC消息"""
        msg = _B_PB2.ClientBindMessage()
        msg.client_info.CopyFrom(self.client.client_info)
        msg.server_info.CopyFrom(dst_server)
        msg.entity_id = self.entity_id

        def _cache_client_message_callback(status):
            if not status:
                self.logger.error(
                        'AvatarEntity._cache_client_message: cache RPC failed')
                self._on_transfer_result(status, callback)
                return
            self._do_transfer_to_server(dst_server,
                    pre_entity_id, content, callback)
            client = self.client
            self.client = None
            self.destroy()
            self._create_post_entity(dst_server, msg, client)

        self.client.transfer_client(msg, callback)