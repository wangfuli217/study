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
#include <sstream>
#include <vector>
#include "nyx_container.h"

namespace nyx {

class nyx_list : public PyObject {
  using ObjectVector = std::vector<PyObject*>;
  ObjectVector* vec_{};
public:
  inline void _init(void) {
    vec_ = new ObjectVector();
  }

  inline void _dealloc(void) {
    if (vec_ != nullptr) {
      _clear();
      delete vec_;
    }
  }

  inline void _clear(void) {
    for (auto* x : *vec_)
      Py_DECREF(x);
    vec_->clear();
  }

  inline Py_ssize_t _size(void) const {
    return vec_->size();
  }

  std::string _repr(void) const {
    std::ostringstream oss;
    oss << "[";
    auto begin = vec_->begin();
    while (begin != vec_->end()) {
      if (PyString_Check(*begin))
        oss << "'" << PyString_AsString(*begin) << "'";
      else
        oss << PyString_AsString(PyObject_Repr(*begin));

      if (++begin != vec_->end())
        oss << ", ";
    }
    oss << "]";

    return oss.str();
  }
};

static int _nyxlist_init(
    nyx_list* self, PyObject* /*args*/, PyObject* /*kwargs*/) {
  self->_init();
  return 0;
}

static void _nyxlist_dealloc(nyx_list* self) {
  self->_dealloc();
}

static PyObject* _nyxlist_repr(nyx_list* self) {
  return Py_BuildValue("s", self->_repr().c_str());
}

static PyObject* _nyxlist_clear(nyx_list* self) {
  self->_clear();
  Py_RETURN_NONE;
}

static PyObject* _nyxlist_size(nyx_list* self) {
  return Py_BuildValue("l", self->_size());
}

static PyMethodDef _nyxlist_methods[] = {
  {"clear", (PyCFunction)_nyxlist_clear, METH_NOARGS, "L.clear() -> clear all items"},
  {"size", (PyCFunction)_nyxlist_size, METH_NOARGS, "L.size() -> return size of list"},
  {nullptr}
};

static PyTypeObject _nyxlist_type = {
  PyObject_HEAD_INIT(nullptr)
  0, // ob_size
  "_nyxcore.nyx_list", // tp_name
  sizeof(nyx_list), // tp_basicsize
  0, // tp_itemsize
  (destructor)_nyxlist_dealloc, // tp_dealloc
  0, // tp_print
  0, // tp_getattr
  0, // tp_setattr
  0, // tp_compare
  (reprfunc)_nyxlist_repr, // tp_repr
  0, // tp_as_number
  0, // tp_as_sequence
  0, // tp_as_mapping
  (hashfunc)PyObject_HashNotImplemented, // tp_hash
  0, // tp_call
  (reprfunc)_nyxlist_repr, // tp_str
  PyObject_GenericGetAttr, // tp_getattro
  0, // tp_setattro
  0, // tp_as_buffer
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, // tp_flags
  "class nyx_list", // tp_doc
  0, // tp_traverse
  0, // tp_clear
  0, // tp_richcompare
  0, // tp_weaklistoffset
  0, // tp_iter
  0, // tp_iternext
  _nyxlist_methods, // tp_methods
  0, // tp_members
  0, // tp_getset
  0, // tp_base
  0, // tp_dict
  0, // tp_descr_get
  0, // tp_descr_set
  0, // tp_dictoffset
  (initproc)_nyxlist_init, // tp_init
  0, // tp_alloc
  PyType_GenericNew, // tp_new
};

void nyx_list_wrap(PyObject* m) {
  if (PyType_Ready(&_nyxlist_type) < 0)
    return;

  auto* _type = reinterpret_cast<PyObject*>(&_nyxlist_type);
  Py_INCREF(_type);
  PyModule_AddObject(m, "nyx_list", _type);
}

}
