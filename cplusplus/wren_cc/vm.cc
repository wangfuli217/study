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
#include <iostream>
#include <sstream>
#include "primitives.hh"
#include "compiler.hh"
#include "vm.hh"

namespace wrencc {

int SymbolTable::ensure(const str_t& name) {
  int existing = get(name);
  if (existing != -1)
    return existing;

  symbols_.push_back(name);
  return Xt::as_type<int>(symbols_.size() - 1);
}

int SymbolTable::add(const str_t& name) {
  if (get(name) != -1)
    return -1;

  symbols_.push_back(name);
  return Xt::as_type<int>(symbols_.size() - 1);
}

int SymbolTable::get(const str_t& name) const {
  for (sz_t i = 0; i < symbols_.size(); ++i) {
    if (symbols_[i] == name)
      return Xt::as_type<int>(i);
  }
  return -1;
}

void SymbolTable::clear(void) {
  symbols_.clear();
}

void SymbolTable::truncate(int count) {
  auto n = Xt::as_type<sz_t>(count);
  ASSERT(n <= symbols_.size(), "cannot truncate to larger size");
  symbols_.resize(n);
}

struct CallFrame {
  int ip{};
  FunctionObject* fn{};
  int stack_start{};

  CallFrame(int ip_arg, FunctionObject* fn_arg, int stack_start_arg) noexcept
    : ip(ip_arg), fn(fn_arg), stack_start(stack_start_arg) {
  }

  inline u8_t get_code(int i) const { return fn->get_code(i); }
  inline const Value& get_constant(int i) const { return fn->get_constant(i); }
};

class Fiber : private UnCopyable {
  std::vector<Value> stack_;
  std::vector<CallFrame> frames_;
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

  void call_function(FunctionObject* fn, int argc = 0) {
    frames_.push_back(CallFrame(0, fn, stack_size() - argc));
  }

  void iter_stacks(std::function<void (const Value&)>&& visit) {
    for (auto& v : stack_)
      visit(v);
  }

  void iter_frames(std::function<void (const CallFrame&)>&& visit) {
    for (auto& c : frames_)
      visit(c);
  }
};

/// VM IMPLEMENTATIONS

static Value _primitive_metaclass_new(VM& vm, Value* args) {
  return InstanceObject::make_instance(vm, args[0].as_class());
}

VM::VM(void) noexcept {
  fiber_ = new Fiber();

  globals_.resize(kMaxGlobals);
  for (int i = 0; i < kMaxGlobals; ++i)
    globals_[i] = nullptr;

  load_core(*this);
}

VM::~VM(void) {
  delete fiber_;
}

void VM::set_primitive(ClassObject* cls, const str_t& name, PrimitiveFn fn) {
  int symbol = methods_.ensure(name);
  cls->set_method(symbol, fn);
}

void VM::set_primitive(ClassObject* cls, const str_t& name, FiberPrimitiveFn fn) {
  int symbol = methods_.ensure(name);
  cls->set_method(symbol, fn);
}

void VM::set_global(ClassObject* cls, const str_t& name) {
  InstanceObject* obj = InstanceObject::make_instance(*this, cls);
  int symbol = global_symbols_.add(name);
  globals_[symbol] = obj;
}

const Value& VM::get_global(const str_t& name) const {
  int symbol = global_symbols_.get(name);
  return globals_[symbol];
}

void VM::pin_object(BaseObject* obj) {
  ASSERT(pinned_.size() < kMaxPinned, "too many pinned objects");
  pinned_.push_back(obj);
}

void VM::unpin_object(BaseObject* obj) {
  ASSERT(pinned_.back() == obj, "unppinning object out of stack order");
  pinned_.pop_back();
}

ClassObject* VM::get_class(const Value& val) const {
#ifdef NAN_TAGGING
  if (val.is_numeric())
    return num_class_;
  if (val.is_object()) {
    switch (val.objtype()) {
    case ObjType::STRING: return str_class_;
    case ObjType::FUNCTION: return fn_class_;
    case ObjType::CLASS: return val.as_class()->meta_class();
    case ObjType::INSTANCE: return val.as_instance()->cls();
    }
  }
  switch (val.tag()) {
  case Tag::NaN: return num_class_;
  case Tag::NIL: return nil_class_;
  case Tag::TRUE:
  case Tag::FALSE: return bool_class_;
  }
#else
  switch (val.type()) {
  case ValueType::NIL: return nil_class_;
  case ValueType::TRUE:
  case ValueType::FALSE: return bool_class_;
  case ValueType::NUMERIC: return num_class_;
  case ValueType::OBJECT:
    switch (val.objtype()) {
    case ObjType::STRING: return str_class_;
    case ObjType::FUNCTION: return fn_class_;
    case ObjType::CLASS: return val.as_class()->meta_class();
    case ObjType::INSTANCE: return val.as_instance()->cls();
    }
    break;
  }
#endif
  return nullptr;
}

Value VM::interpret(FunctionObject* fn) {
  Fiber* fiber = fiber_;
  fiber->call_function(fn, 0);

#define PUSH(v) fiber->push(v)
#define POP()   fiber->pop()
#define PEEK()  fiber->peek_value()
#define RDARG() frame->get_code(frame->ip++)

  auto* frame = &fiber->peek_frame();
  for (;;) {
    switch (auto c = Xt::as_type<Code>(frame->get_code(frame->ip++))) {
    case Code::CONSTANT: PUSH(frame->get_constant(RDARG())); break;
    case Code::NIL: PUSH(nullptr); break;
    case Code::FALSE: PUSH(false); break;
    case Code::TRUE: PUSH(true); break;
    case Code::CLASS:
    case Code::SUBCLASS:
      {
        bool is_subclass = c == Code::SUBCLASS;
        ClassObject* superclass;
        if (is_subclass)
          superclass = POP().as_class();
        else
          superclass = obj_class_;

        ClassObject* cls = ClassObject::make_class(*this, superclass);

        // assume the first class being defined is Object
        if (obj_class_ == nullptr)
          obj_class_ = cls;

        // define `new` method on the metaclass.
        int new_symbol = methods_.ensure("new");
        cls->meta_class()->set_method(new_symbol, _primitive_metaclass_new);
        PUSH(cls);
      } break;
    case Code::METACLASS: PUSH(PEEK().as_class()->meta_class()); break;
    case Code::METHOD:
      {
        int symbol = RDARG();
        int constant = RDARG();
        ClassObject* cls = PEEK().as_class();

        FunctionObject* body = frame->get_constant(constant).as_function();
        cls->set_method(symbol, body);
      } break;
    case Code::LOAD_LOCAL:
      {
        int local = RDARG();
        PUSH(fiber->get_value(frame->stack_start + local));
      } break;
    case Code::STORE_LOCAL:
      {
        int local = RDARG();
        fiber->set_value(frame->stack_start + local, PEEK());
      } break;
    case Code::LOAD_GLOBAL:
      {
        int global = RDARG();
        PUSH(globals_[global]);
      } break;
    case Code::STORE_GLOBAL:
      {
        int global = RDARG();
        globals_[global] = PEEK();
      } break;
    case Code::DUP: PUSH(PEEK()); break;
    case Code::POP: POP(); break;
    case Code::CALL_0:
    case Code::CALL_1:
    case Code::CALL_2:
    case Code::CALL_3:
    case Code::CALL_4:
    case Code::CALL_5:
    case Code::CALL_6:
    case Code::CALL_7:
    case Code::CALL_8:
    case Code::CALL_9:
    case Code::CALL_10:
      {
        int argc = c - Code::CALL_0 + 1;
        int symbol = RDARG();
        const Value& receiver = fiber->get_value(fiber->stack_size() - argc);

        ClassObject* cls = get_class(receiver);
        auto& method = cls->get_method(symbol);
        switch (method.type) {
        case MethodType::NONE:
          std::cerr
            << "receiver: `" << receiver << "` "
            << "does not implement method `" << methods_.get_name(symbol) << "`"
            << std::endl;
          std::exit(-1); break;
        case MethodType::PRIMITIVE:
          {
            Value* args = fiber->values_at(fiber->stack_size() - argc);
            // argc +1 to include the receiver since that's in the args array
            Value result = method.primitive(*this, args);

            fiber->set_value(fiber->stack_size() - argc, result);
            // discard the stack slots for the arguments (but leave one for
            // the result)
            fiber->resize_stack(fiber->stack_size() - (argc - 1));
          } break;
        case MethodType::FIBER:
          {
            Value* args = fiber->values_at(fiber->stack_size() - argc);
            method.fiber_primitive(*this, *fiber, args);

            frame = &fiber->peek_frame();
          } break;
        case MethodType::BLOCK:
          fiber->call_function(method.fn, argc);
          frame = &fiber->peek_frame();
          break;
        }
      } break;
    case Code::JUMP: frame->ip += RDARG(); break;
    case Code::LOOP:
      {
        // the loop body's result is on the top of the stack, since we are
        // looping and running the body again, discard it
        POP();

        // jump back to the top of the loop
        frame->ip -= RDARG();
      } break;
    case Code::JUMP_IF:
      {
        int offset = RDARG();
        const Value& cond = POP();

        if (cond.is_falsely())
          frame->ip += offset;
      } break;
    case Code::AND:
      {
        int offset = RDARG();
        const Value& cond = PEEK();

        // false and nil is the only falsey value
        if (!cond.is_falsely())
          POP(); // discard the condition and evaluate the right hand side
        else
          frame->ip += offset; // short-circuit the right hand side
      } break;
    case Code::IS:
      {
        const Value& cls = POP();
        const Value& obj = POP();

        ClassObject* actual = get_class(obj);
        PUSH(actual == cls.as_class());
      } break;
    case Code::END:
      {
        const Value& r = POP();
        fiber->pop_frame();

        if (fiber->empty_frame())
          return r;

        if (fiber->stack_size() <= frame->stack_start)
          PUSH(r);
        else
          fiber->set_value(frame->stack_start, r);
        fiber->resize_stack(frame->stack_start + 1);

        frame = &fiber->peek_frame();
      } break;
    }
  }
  return nullptr;
}

void VM::interpret(const str_t& source_bytes) {
  auto* fn = compile(*this, source_bytes);

  if (fn != nullptr)
    interpret(fn);
}

void VM::call_function(Fiber& fiber, FunctionObject* fn, int argc) {
  fiber.call_function(fn, argc);
}

void VM::collect(void) {
  // global variables
  for (int i = 0; i < global_symbols_.count(); ++i)
    mark_value(globals_[i]);
  // pinned objects
  for (auto& o : pinned_)
    mark_value(o);
  // stack functions
  fiber_->iter_frames([this](const CallFrame& f) { mark_object(f.fn); });
  // stack variables
  fiber_->iter_stacks([this](const Value& v) { mark_value(v); });

  // collect any unmarked objects
  for (auto it = objects_.begin(); it != objects_.end();) {
    if (!((*it)->flag() & ObjFlag::MARKED)) {
      free_object(*it);
      objects_.erase(it++);
    }
    else {
      ObjFlag f = (*it)->flag();
      (*it)->set_flag(f & ~ObjFlag::MARKED);
      ++it;
    }
  }
  total_allocated_ = objects_.size();
}

void VM::free_object(BaseObject* obj) {
  std::cout
    << "`" << Xt::cast<void>(obj) << "` free object "
    << "`" << obj->stringify() << "`" << std::endl;

  delete obj;
}

void VM::append_object(BaseObject* obj) {
  if (objects_.size() > next_gc_) {
    collect();
    next_gc_ = total_allocated_ * 3 / 2;
  }

  objects_.push_back(obj);
}

void VM::mark_object(BaseObject* obj) {
  if (obj == nullptr)
    return;
  if (obj->flag() & ObjFlag::MARKED)
    return;

  obj->set_flag(obj->flag() | ObjFlag::MARKED);

  obj->gc_mark(*this);
}

void VM::mark_value(const Value& val) {
  if (val.is_object())
    mark_object(val.as_object());
}

}
