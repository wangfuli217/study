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

import os
import platform
import sys

class ExtendableType(type):
    def __new__(cls, name, bases, dict_info):
        if name == '__extend__':
            for cls in bases:
                for key, value in dict_info.iteritems():
                    if key == '__module__':
                        continue
                    setattr(cls, key, value)
            return None
        else:
            return super(ExtendableType, cls).__new__(cls, name, bases, dict_info)

class AttrProxy(object):
    def __init__(self, attr):
        super(AttrProxy, self).__setattr__('attr', attr)

    def __getattr__(self, name):
        attr = super(AttrProxy, self).__getattribute__('attr')
        value = attr.get(name)
        if not value:
            raise AttributeError('AttrProxy has no attribute(%s)' % name)
        return value

    def __setattr__(self, name, value):
        attr = super(AttrProxy, self).__getattribute__('attr')
        attr[name] = value

def singleton(cls):
    _instances = {}

    def get_instance(*args, **kwargs):
        if cls not in _instances:
            _instances[cls] = cls(*args, **kwargs)
        return _instances[cls]
    return get_instance

def get_architecture():
    return platform.architecture()[0]

def add_syspath(new_path):
    """给Python的sys.path添加一个目录

    Returns:
        1表示添加成功
        0表示new_path已经在sys.path中了
        -1表示new_path目录不存在
    """
    if not os.path.exists(new_path):
        return -1
    new_abspath = os.path.abspath(new_path)
    if sys.platform == 'win32':
        new_abspath = new_abspath.lower()
        for path in sys.path:
            abspath = os.path.abspath(path).lower()
            if new_abspath in (abspath, abspath + os.sep):
                return 0
    sys.path.append(new_abspath)
    return 1

if __name__ == '__main__':
    a = AttrProxy({})
    a.name = 'John'
    a.age = 20
    a.sex = 'male'
    print 'a.name=`%s`, a.age=%d, a.sex=`%s`' % (a.name, a.age, a.sex)

    @singleton
    class UselessName(object):
        def __init__(self):
            self._name = 'Useless'

        def set_name(self, name):
            self._name = name

        def show(self):
            print 'UselessName.show - name=`%s`' % self._name

    UselessName().set_name('Jack.Gus')
    UselessName().show()