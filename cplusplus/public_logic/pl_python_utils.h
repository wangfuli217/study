#pragma once

#include <map>
#include <vector>
#include <Python.h>
#include <boost/enable_shared_from_this.hpp>
#include <boost/python.hpp>

namespace py = ::boost::python;

namespace public_logic {

inline bool hasattr(const py::object& obj, const std::string& name) {
	return PyObject_HasAttrString(obj.ptr(), name.c_str());
}

template <typename T>
std::vector<T> list_as_vector(const py::list& obj) {
	std::size_t n = py::len(obj);
	std::vector<T> r(n);
	for (std::size_t i = 0; i < n; ++i)
		r[i] = py::extract<T>(obj[i])();

	return r;
}

template <typename VectorType>
py::list vector_as_list(const VectorType& v) {
	py::list r;
	for (auto& i : v)
		r.append(i);
	return r;
}

template <typename Key, typename Value>
std::map<Key, Value> dict_as_map(const py::dict& obj) {
	std::map<Key, Value> r;
	py::list keys = obj.keys();
	std::size_t n = py::len(keys);
	for (std::size_t i = 0; i < n; ++i) {
		auto key = py::extract<Key>(keys[i])();
		auto val = py::extract<Value>(obj[key])();
		r[key] = val;
	}

	return r;
}

template <typename MapType>
py::dict map_as_dict(const MapType& m) {
	py::dict r;
	for (auto& i : m)
		r[i.first] = i.second;
	return r;
}

template <typename T>
void register_shared_ptr(void) {
	py::register_ptr_to_python<boost::shared_ptr<T>>();
	py::register_ptr_to_python<boost::shared_ptr<const T>>();
	py::implicitly_convertible<boost::shared_ptr<T>, boost::shared_ptr<const T>>();
}

class AcquirePyGIL : private boost::noncopyable {
	PyGILState_STATE state_;
public:
	AcquirePyGIL(void) {
		state_ = PyGILState_Ensure();
	}

	~AcquirePyGIL(void) {
		PyGILState_Release(state_);
	}
};

}

#define _PL_TRY {\
	AcquirePyGIL gil;\
	try

#define _PL_WITHOUT_GIL_TRY {\
	try

#define _PL_END_TRY\
	catch (const py::error_already_set&) {\
		PyErr_Print();\
	}\
	catch (...) {\
	}\
}