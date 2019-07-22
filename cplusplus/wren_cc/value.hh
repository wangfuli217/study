// Copyright (c) 2019 ASMlover. All rights reserved.
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
#pragma once

#include <functional>
#include <variant>
#include <vector>
#include "common.hh"
#include "wren.hh"

namespace wrencc {

enum class ValueType : u8_t {
  NIL,
  TRUE,
  FALSE,
  NUMERIC,
  OBJECT,
};

enum class ObjType : u8_t {
  STRING,
  LIST,
  FUNCTION,
  UPVALUE,
  CLOSURE,
  FIBER,
  CLASS,
  INSTANCE,
};

enum ObjFlag {
  UNMARK = 0x00,
  MARKED = 0x01,
};

class WrenVM;
class BaseObject;

class NilObject;
class BooleanObject;
class NumericObject;
class StringObject;
class ListObject;
class FunctionObject;
class UpvalueObject;
class ClosureObject;
class FiberObject;
class ClassObject;
class InstanceObject;

class BaseObject : private UnCopyable {
  ObjType type_ : 3;
  ObjFlag flag_ : 1;
public:
  BaseObject(ObjType type) noexcept : type_(type), flag_(ObjFlag::UNMARK) {}
  virtual ~BaseObject(void) {}

  inline ObjType type(void) const { return type_; }
  inline ObjFlag flag(void) const { return flag_; }
  template <typename T> inline void set_flag(T f) { flag_ = Xt::as_type<ObjFlag>(f); }

  virtual str_t stringify(void) const = 0;
  virtual void gc_mark(WrenVM& vm) {}
};

enum Tag {
  MASK = 7,

  NaN = 0,
  NIL,
  FALSE,
  TRUE,
  UNUSED1,
  UNUSED2,
  UNUSED3,
  UNUSED4,
};

class TagValue final : public Copyable {
  static constexpr u64_t kSignBit = 1llu << 63;
  static constexpr u64_t kQNaN = 0x7ffc000000000000llu;

  union {
    double num_;
    u64_t bits_;
  };

  template <typename T> inline void set_decimal(T x) {
    num_ = Xt::as_type<double>(x);
  }

  inline bool check(ObjType type) const {
    return is_object() && as_object()->type() == type;
  }
public:
  TagValue(void) noexcept {}
  TagValue(nil_t) noexcept { bits_ = (kQNaN | Tag::NIL); }
  TagValue(bool b) noexcept { bits_ = b ? (kQNaN | Tag::TRUE) : (kQNaN | Tag::FALSE); }
  TagValue(i8_t n) noexcept { set_decimal(n); }
  TagValue(u8_t n) noexcept { set_decimal(n); }
  TagValue(i16_t n) noexcept { set_decimal(n); }
  TagValue(u16_t n) noexcept { set_decimal(n); }
  TagValue(i32_t n) noexcept { set_decimal(n); }
  TagValue(u32_t n) noexcept { set_decimal(n); }
  TagValue(i64_t n) noexcept { set_decimal(n); }
  TagValue(u64_t n) noexcept { set_decimal(n); }
  TagValue(float n) noexcept { set_decimal(n); }
  TagValue(double d) noexcept { num_ = d; }
  TagValue(BaseObject* o) noexcept { bits_ = (kSignBit | kQNaN | (u64_t)(o)); }

  inline bool operator==(const TagValue& r) const noexcept { return bits_ == r.bits_; }
  inline bool operator!=(const TagValue& r) const noexcept { return bits_ != r.bits_; }

  inline int tag(void) const { return Xt::as_type<int>(bits_ & Tag::MASK); }
  inline ObjType objtype(void) const { return as_object()->type(); }

  inline bool is_nil(void) const { return bits_ == (kQNaN | Tag::NIL); }
  inline bool is_boolean(void) const { return (bits_ == (kQNaN | Tag::TRUE)) || (bits_ == (kQNaN | Tag::FALSE)); }
  inline bool is_numeric(void) const { return (bits_ & kQNaN) != kQNaN; }
  inline bool is_object(void) const { return (bits_ & (kSignBit | kQNaN)) == (kSignBit | kQNaN); }
  inline bool is_string(void) const { return check(ObjType::STRING); }
  inline bool is_list(void) const { return check(ObjType::LIST); }
  inline bool is_function(void) const { return check(ObjType::FUNCTION); }
  inline bool is_upvalue(void) const { return check(ObjType::UPVALUE); }
  inline bool is_closure(void) const { return check(ObjType::CLOSURE); }
  inline bool is_fiber(void) const { return check(ObjType::FIBER); }
  inline bool is_class(void) const { return check(ObjType::CLASS); }
  inline bool is_instance(void) const { return check(ObjType::INSTANCE); }

  inline bool is_falsely(void) const { return is_nil() || bits_ == (kQNaN | Tag::FALSE); }

  inline bool as_boolean(void) const { return bits_ == (kQNaN | Tag::TRUE); }
  inline double as_numeric(void) const { return num_; }
  inline BaseObject* as_object(void) const { return (BaseObject*)(bits_ & ~(kSignBit | kQNaN)); }

  StringObject* as_string(void) const;
  const char* as_cstring(void) const;
  ListObject* as_list(void) const;
  FunctionObject* as_function(void) const;
  UpvalueObject* as_upvalue(void) const;
  ClosureObject* as_closure(void) const;
  FiberObject* as_fiber(void) const;
  ClassObject* as_class(void) const;
  InstanceObject* as_instance(void) const;

  str_t stringify(void) const;
};

class ObjValue final : public Copyable {
  ValueType type_{};
  double num_{};
  BaseObject* obj_{};

  inline bool check(ObjType type) const { return is_object() && obj_->type() == type; }

  template <typename T> inline double to_decimal(T x) {
    return Xt::as_type<double>(x);
  }
public:
  ObjValue(void) noexcept : type_(ValueType::OBJECT) {}
  ObjValue(nil_t) noexcept : type_(ValueType::NIL) {}
  ObjValue(bool b) noexcept : type_(b ? ValueType::TRUE : ValueType::FALSE) {}
  ObjValue(i8_t n) noexcept : type_(ValueType::NUMERIC), num_(to_decimal(n)) {}
  ObjValue(u8_t n) noexcept : type_(ValueType::NUMERIC), num_(to_decimal(n)) {}
  ObjValue(i16_t n) noexcept : type_(ValueType::NUMERIC), num_(to_decimal(n)) {}
  ObjValue(u16_t n) noexcept : type_(ValueType::NUMERIC), num_(to_decimal(n)) {}
  ObjValue(i32_t n) noexcept : type_(ValueType::NUMERIC), num_(to_decimal(n)) {}
  ObjValue(u32_t n) noexcept : type_(ValueType::NUMERIC), num_(to_decimal(n)) {}
  ObjValue(i64_t n) noexcept : type_(ValueType::NUMERIC), num_(to_decimal(n)) {}
  ObjValue(u64_t n) noexcept : type_(ValueType::NUMERIC), num_(to_decimal(n)) {}
  ObjValue(float n) noexcept : type_(ValueType::NUMERIC), num_(to_decimal(n)) {}
  ObjValue(double d) noexcept : type_(ValueType::NUMERIC), num_(d) {}
  ObjValue(BaseObject* o) noexcept : type_(ValueType::OBJECT), obj_(o) {}

  inline ValueType type(void) const { return type_; }
  inline ObjType objtype(void) const { return obj_->type(); }
  inline bool is_valid(void) const { return type_ != ValueType::OBJECT || obj_ != nullptr; }

  inline bool is_nil(void) const { return type_ == ValueType::NIL; }
  inline bool is_boolean(void) const { return type_ == ValueType::TRUE || type_ == ValueType::FALSE; }
  inline bool is_numeric(void) const { return type_ == ValueType::NUMERIC; }
  inline bool is_object(void) const { return type_ == ValueType::OBJECT; }
  inline bool is_string(void) const { return check(ObjType::STRING); }
  inline bool is_list(void) const { return check(ObjType::LIST); }
  inline bool is_function(void) const { return check(ObjType::FUNCTION); }
  inline bool is_upvalue(void) const { return check(ObjType::UPVALUE); }
  inline bool is_closure(void) const { return check(ObjType::CLOSURE); }
  inline bool is_fiber(void) const { return check(ObjType::FIBER); }
  inline bool is_class(void) const { return check(ObjType::CLASS); }
  inline bool is_instance(void) const { return check(ObjType::INSTANCE); }

  inline bool is_falsely(void) const { return is_nil() || type_ == ValueType::FALSE; }

  inline BaseObject* as_object(void) const { return obj_; }
  inline bool as_boolean(void) const { return type_ == ValueType::TRUE; }
  inline double as_numeric(void) const { return num_; }

  StringObject* as_string(void) const;
  const char* as_cstring(void) const;
  ListObject* as_list(void) const;
  FunctionObject* as_function(void) const;
  UpvalueObject* as_upvalue(void) const;
  ClosureObject* as_closure(void) const;
  FiberObject* as_fiber(void) const;
  ClassObject* as_class(void) const;
  InstanceObject* as_instance(void) const;

  bool operator==(const ObjValue& r) const noexcept;
  bool operator!=(const ObjValue& r) const noexcept;
  str_t stringify(void) const;
};

#ifdef NAN_TAGGING
using Value = TagValue;
#else
using Value = ObjValue;
#endif

std::ostream& operator<<(std::ostream& out, const Value& val);

class StringObject final : public BaseObject {
  int size_{};
  char* value_{};

  StringObject(char c) noexcept;
  StringObject(const char* s, int n, bool replace_owner = false) noexcept;
  virtual ~StringObject(void);
public:
  inline int size(void) const { return size_; }
  inline const char* cstr(void) const { return value_; }
  inline char operator[](int i) const { return value_[i]; }
  inline char& operator[](int i) { return value_[i]; }

  virtual str_t stringify(void) const override;

  static StringObject* make_string(WrenVM& vm, char c);
  static StringObject* make_string(WrenVM& vm, const char* s, int n);
  static StringObject* make_string(WrenVM& vm, const str_t& s);
  static StringObject* make_string(WrenVM& vm, StringObject* s1, StringObject* s2);
};

class ListObject final : public BaseObject {
  std::vector<Value> elements_;

  ListObject(int num_elements = 0) noexcept;
public:
  inline int count(void) const { return Xt::as_type<int>(elements_.size()); }
  inline const Value* elements(void) const { return elements_.data(); }
  inline const Value& get_element(int i) const { return elements_[i]; }
  inline void set_element(int i, const Value& e) { elements_[i] = e; }
  inline void clear(void) { elements_.clear(); }
  inline void add_element(const Value& e) { elements_.push_back(e); }
  inline void insert(int i, const Value& e) { elements_.insert(elements_.begin() + i, e); }
  inline Value remove(int i) {
    Value removed = elements_[i];
    elements_.erase(elements_.begin() + i);
    return removed;
  }

  virtual str_t stringify(void) const override;
  virtual void gc_mark(WrenVM& vm) override;

  static ListObject* make_list(WrenVM& vm, int num_elements = 0);
};

// stores debugging information for a function used for things like stack
// traces
class DebugObject final : private UnCopyable {
  // the name of the function, heap allocated and owned by the FunctionObject
  str_t name_{};

  // the name of the source file where this function was defined, an
  // [StringObject] because this will be shared among all functions defined
  // in the same file
  str_t source_path_{};

  // an array of line numbers, there is one element in this array for each
  // bytecode in the function's bytecode array, the value of that element
  // is the line in the source code that generated that instruction
  std::vector<int> source_lines_;
public:
  DebugObject(const str_t& name,
      const str_t& source_path, int* source_lines, int lines_count) noexcept;

  inline const str_t& name(void) const { return name_; }
  inline const str_t& source_path(void) const { return source_path_; }
  inline int get_line(int i) const { return source_lines_[i]; }
};

class FunctionObject final : public BaseObject {
  int num_upvalues_{};
  std::vector<u8_t> codes_;
  std::vector<Value> constants_;
  DebugObject debug_;

  FunctionObject(int num_upvalues,
      u8_t* codes, int codes_count,
      const Value* constants, int constants_count,
      const str_t& source_path, const str_t& debug_name,
      int* source_lines, int lines_count) noexcept;
public:
  inline int num_upvalues(void) const { return num_upvalues_; }
  inline void set_num_upvalues(int num_upvalues) { num_upvalues_ = num_upvalues; }
  inline const u8_t* codes(void) const { return codes_.data(); }
  inline void set_codes(const std::vector<u8_t>& codes) { codes_ = codes; }
  inline const Value* constants(void) const { return constants_.data(); }
  inline void set_constants(const std::vector<Value>& constants) { constants_ = constants; }
  inline int codes_count(void) const { return Xt::as_type<int>(codes_.size()); }
  inline int constants_count(void) const { return Xt::as_type<int>(constants_.size()); }
  inline u8_t get_code(int i) const { return codes_[i]; }
  inline const Value& get_constant(int i) const { return constants_[i]; }
  inline const DebugObject& debug(void) const { return debug_; }

  template <typename T> inline void set_code(int i, T c) {
    codes_[i] = Xt::as_type<u8_t>(c);
  }

  int get_argc(int ip) const;

  virtual str_t stringify(void) const override;
  virtual void gc_mark(WrenVM& vm) override;

  static FunctionObject* make_function(WrenVM& vm, int num_upvalues,
      u8_t* codes, int codes_count,
      const Value* constants, int constants_count,
      const str_t& source_path, const str_t& debug_name,
      int* source_lines, int lines_count);
};

// the dynamically allocated data structure for a variable that has been
// used by a closure. whenever a function accesses a variable declared in
// an enclosing function, it will get to it through this.
//
// an upvalue can be either `closed` or `open`, an open upvalue points
// directly to a [value] that is still stored on the fiber's stack because
// the local variable is still in scope in the function where it's declared.
//
// when that local variable goes out of scope, the upvalue pointing to it
// will be closed. when that happens, the value gets copied off the stack
// into the upvalue itself, that way, it can have a longer lifetime than
// the stack variable.
class UpvalueObject final : public BaseObject {
  Value* value_{}; // pointer to the variable this upvalue is referencing

  // if the upvalue is closed then the closed-over value will be hoisted out
  // of the stack into here. [value] will then be changed to point to this.
  Value closed_{};

  // open upvalues are stored in a linked list by the fiber. this points to
  // the next upvalue in that list.
  UpvalueObject* next_{};

  UpvalueObject(Value* value, UpvalueObject* next = nullptr) noexcept;
  virtual ~UpvalueObject(void) {}
public:
  inline Value* value(void) const { return value_; }
  inline Value* value_asptr(void) const { return value_; }
  inline const Value& value_asref(void) const { return *value_; }
  inline void set_value(Value* value) { value_ = value; }
  inline void set_value(const Value& value) { *value_ = value; }
  inline const Value& closed(void) const { return closed_; }
  inline Value* closed_asptr(void) const { return const_cast<Value*>(&closed_); }
  inline const Value& closed_asref(void) const { return closed_; }
  inline void set_closed(const Value& closed) { closed_ = closed; }
  inline void set_closed(Value* closed) { closed_ = *closed; }
  inline UpvalueObject* next(void) const { return next_; }
  inline void set_next(UpvalueObject* next) { next_ = next; }

  virtual str_t stringify(void) const override;
  virtual void gc_mark(WrenVM& vm) override;

  static UpvalueObject* make_upvalue(
      WrenVM& vm, Value* value, UpvalueObject* next = nullptr);
};

class ClosureObject final : public BaseObject {
  FunctionObject* fn_{}; // function that this closure is an instance of
  UpvalueObject** upvalues_{}; // the upvalues this function has closed over

  ClosureObject(FunctionObject* fn) noexcept;
  virtual ~ClosureObject(void);
public:
  inline FunctionObject* fn(void) const { return fn_; }
  inline bool has_upvalues(void) const { return upvalues_ != nullptr; }
  inline UpvalueObject** upvalues(void) const { return upvalues_; }
  inline UpvalueObject* get_upvalue(int i) const { return upvalues_[i]; }
  inline void set_upvalue(int i, UpvalueObject* upvalue) { upvalues_[i] = upvalue; }

  virtual str_t stringify(void) const override;
  virtual void gc_mark(WrenVM& vm) override;

  static ClosureObject* make_closure(WrenVM& vm, FunctionObject* fn);
};

struct CallFrame {
  const u8_t* ip{}; // pointer to the current instruction in the function's body
  BaseObject* fn{}; // the function or closure being executed

  // index of the first stack slot used by this call frame, this will contain
  // the receiver, followed by the function's parameters, then local variables
  // and temporaries
  int stack_start{};

  CallFrame(const u8_t* _ip, BaseObject* _fn, int _stack_start) noexcept
    : ip(_ip), fn(_fn), stack_start(_stack_start) {
  }
};

class FiberObject final : public BaseObject {
  static constexpr sz_t kDefaultCap = 256;

  std::vector<Value> stack_;
  std::vector<CallFrame> frames_;

  // pointer to the first node in the linked list of open upvalues that are
  // pointing to values still on the stack. the head of the list will be the
  // upvalue closest to the top of the stack, and then the list works downwards
  UpvalueObject* open_upvlaues_{};

  FiberObject(void) noexcept : BaseObject(ObjType::FIBER) {
    stack_.reserve(kDefaultCap);
  }
  virtual ~FiberObject(void) {}
public:
  inline void reset(void) {
    stack_.clear();
    frames_.clear();
  }

  inline Value* values_at(int i) { return &stack_[i]; }
  inline void resize_stack(int n) { stack_.resize(n); }
  inline int stack_size(void) const { return Xt::as_type<int>(stack_.size()); }
  inline int frame_size(void) const { return Xt::as_type<int>(frames_.size()); }
  inline CallFrame& peek_frame(void) { return frames_.back(); }
  inline void pop_frame(void) { frames_.pop_back(); }
  inline bool empty_frame(void) const { return frames_.empty(); }
  inline Value& get_value(int i) { return stack_[i]; }
  inline const Value& get_value(int i) const { return stack_[i]; }
  inline void set_value(int i, const Value& v) { stack_[i] = v; }

  inline const Value& peek_value(int distance = 0) const {
    return stack_[stack_.size() - 1 - distance];
  }

  inline void push(const Value& v) {
    stack_.push_back(v);
  }

  inline Value pop(void) {
    Value v = stack_.back();
    stack_.pop_back();
    return v;
  }

  void call_function(BaseObject* fn, int argc = 0);
  UpvalueObject* capture_upvalue(WrenVM& vm, int slot);
  void close_upvalue(void);
  void close_upvalues(int slot);

  void riter_frames(
      std::function<void (const CallFrame&, FunctionObject*)>&& visit);

  virtual str_t stringify(void) const override;
  virtual void gc_mark(WrenVM& vm) override;

  static FiberObject* make_fiber(WrenVM& vm);
};

enum class PrimitiveResult {
  VALUE,  // a normal value has been returned
  ERROR,  // a runtime error occurred
  CALL,   // a new callframe has been pushed
};

using PrimitiveFn = std::function<PrimitiveResult (WrenVM&, FiberObject*, Value*)>;

enum class MethodType {
  PRIMITIVE,// a primitive method implemented in C that immediatelt returns a Value
  FOREIGN,  // a externally-defined C++ native method
  BLOCK,    // a normal user-defined method

  NONE,     // no method for the given symbol
};

struct Method {
  MethodType type{MethodType::NONE};
  std::variant<PrimitiveFn, WrenForeignFn, BaseObject*> m_{};

  inline const PrimitiveFn& primitive(void) const { return std::get<PrimitiveFn>(m_); }
  inline void set_primitive(const PrimitiveFn& fn) { m_ = fn; }
  inline const WrenForeignFn& foreign(void) const { return std::get<WrenForeignFn>(m_); }
  inline void set_foreign(const WrenForeignFn& fn) { m_ = fn; }
  inline BaseObject* fn(void) const { return std::get<BaseObject*>(m_); }
  inline void set_fn(BaseObject* fn) { m_ = fn; }

  Method(void) noexcept {}
};

class ClassObject final : public BaseObject {
  static constexpr sz_t kMaxMethods = 256;

  ClassObject* meta_class_{};
  ClassObject* superclass_{};
  int num_fields_{};
  std::vector<Method> methods_{kMaxMethods};

  ClassObject(void) noexcept;
  ClassObject(ClassObject* meta_class,
      ClassObject* supercls = nullptr, int num_fields = 0) noexcept;
public:
  inline ClassObject* meta_class(void) const { return meta_class_; }
  inline void set_meta_class(ClassObject* meta_class) { meta_class_ = meta_class; }
  inline ClassObject* superclass(void) const { return superclass_; }
  inline int num_fields(void) const { return num_fields_; }
  inline int methods_count(void) const { return Xt::as_type<int>(methods_.size()); }
  inline Method& get_method(int i) { return methods_[i]; }
  inline void set_method(int i, const PrimitiveFn& fn) {
    methods_[i].type = MethodType::PRIMITIVE;
    methods_[i].set_primitive(fn);
  }
  inline void set_method(int i, const WrenForeignFn& fn) {
    methods_[i].type = MethodType::FOREIGN;
    methods_[i].set_foreign(fn);
  }
  inline void set_method(int i, BaseObject* fn) {
    methods_[i].type = MethodType::BLOCK;
    methods_[i].set_fn(fn);
  }
  inline void set_method(int i, MethodType fn_type, BaseObject* fn) {
    methods_[i].type = fn_type;
    methods_[i].set_fn(fn);
  }

  void bind_superclass(ClassObject* superclass);
  void bind_method(FunctionObject* fn);
  void bind_method(int i, int method_type, const Value& fn);

  virtual str_t stringify(void) const override;
  virtual void gc_mark(WrenVM& vm) override;

  static ClassObject* make_single_class(WrenVM& vm);
  static ClassObject* make_class(WrenVM& vm,
      ClassObject* superclass = nullptr, int num_fields = 0);
};

class InstanceObject final : public BaseObject {
  ClassObject* cls_{};
  std::vector<Value> fields_;

  InstanceObject(ClassObject* cls) noexcept;
public:
  inline ClassObject* cls(void) const { return cls_; }
  inline const Value& get_field(int i) const { return fields_[i]; }
  inline void set_field(int i, const Value& v) { fields_[i] = v; }

  virtual str_t stringify(void) const override;
  virtual void gc_mark(WrenVM& vm) override;

  static InstanceObject* make_instance(WrenVM& vm, ClassObject* cls);
};

}
