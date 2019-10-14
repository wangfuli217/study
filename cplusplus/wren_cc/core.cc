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
#include <cerrno>
#include <cmath>
#include <iostream>
#include "value.hh"
#include "vm.hh"
#include "primitive.hh"
#include "core.hh"

namespace wrencc {

#define DEF_PRIMITIVE_FN(fn, i)\
DEF_PRIMITIVE(fn##i) {\
  return call_function(vm, args, i);\
}

#include "core.wren.hh"

DEF_PRIMITIVE(nil_not) {
  RETURN_VAL(true);
}

DEF_PRIMITIVE(nil_tostring) {
  RETURN_VAL(StringObject::make_string(vm, "nil"));
}

DEF_PRIMITIVE(bool_tostring) {
  if (args[0].as_boolean())
    RETURN_VAL(StringObject::make_string(vm, "true"));
  else
    RETURN_VAL(StringObject::make_string(vm, "false"));
}

DEF_PRIMITIVE(bool_not) {
  RETURN_VAL(!args[0].as_boolean());
}

DEF_PRIMITIVE(class_name) {
  ClassObject* cls = args[0].as_class();
  RETURN_VAL(cls->name());
}

DEF_PRIMITIVE(class_supertype) {
  ClassObject* cls = args[0].as_class();

  // Object has no superclass
  ClassObject* superclass = cls->superclass();
  if (superclass == nullptr)
    RETURN_VAL(nullptr);
  RETURN_VAL(superclass);
}

DEF_PRIMITIVE(class_tostring) {
  RETURN_VAL(args[0].as_class()->name());
}

DEF_PRIMITIVE(fiber_new) {
  if (!validate_function(vm, args[1], "Argument"))
    return PrimitiveResult::FIBER;

  FiberObject* new_fiber = FiberObject::make_fiber(vm, args[1].as_object());

  // the compiler expect the first slot of a function to hold the receiver.
  // since a fiber's stack is invoked directly, it does not have one, so
  // put it in here
  new_fiber->push(nullptr);

  RETURN_VAL(new_fiber);
}

DEF_PRIMITIVE(fiber_abort) {
  vm.fiber()->set_error(args[1]);

  // if the error is explicitly nil, it's not really an abort
  return args[1].is_nil() ? PrimitiveResult::VALUE : PrimitiveResult::FIBER;
}

static PrimitiveResult run_fiber(
    WrenVM& vm, FiberObject* fiber, Value* args, bool is_call, bool has_value) {
  // transfer execution to [fiber] coming from the current fiber whose stack
  // has [args]
  //
  // [is_call] is true if [fiber] is being called and not transferred
  //
  // [has_value] is true if a value in [args] is being passed to the new fiber
  // otherwise `nil` is implicitly being passed

  if (is_call) {
    if (fiber->caller() != nullptr)
      RETURN_ERR("fiber has already been called");

    // remember who ran it
    fiber->set_caller(vm.fiber());
  }

  if (fiber->empty_frame()) {
    vm.fiber()->set_error(StringObject::format(vm,
        "cannot $ a finished fiber", is_call ? "call" : "transfer to"));
    return PrimitiveResult::FIBER;
  }
  if (!fiber->error().is_nil()) {
    vm.fiber()->set_error(StringObject::format(vm,
        "cannot $ an aborted fiber", is_call ? "call" : "transfer to"));
    return PrimitiveResult::FIBER;
  }

  // when the calling fiber resumes, we'll store the result of the call in its
  // stack, if the call has two arguments (the fiber and the value), we only
  // need one slot for the result, so discard the other slot now
  if (has_value)
    vm.fiber()->pop();
  // if the fiber was paused, make yield() or transfer() return the result
  if (fiber->stack_size() > 0)
    fiber->set_value(fiber->stack_size() - 1, has_value ? args[1] : nullptr);
  vm.set_fiber(fiber);

  return PrimitiveResult::FIBER;
}

DEF_PRIMITIVE(fiber_call) {
  return run_fiber(vm, args[0].as_fiber(), args, true, false);
}

DEF_PRIMITIVE(fiber_call1) {
  return run_fiber(vm, args[0].as_fiber(), args, true, true);
}

DEF_PRIMITIVE(fiber_current) {
  RETURN_VAL(vm.fiber());
}

DEF_PRIMITIVE(fiber_error) {
  RETURN_VAL(args[0].as_fiber()->error());
}

DEF_PRIMITIVE(fiber_isdone) {
  FiberObject* run_fiber = args[0].as_fiber();
  RETURN_VAL(run_fiber->empty_frame() || !run_fiber->error().is_nil());
}

DEF_PRIMITIVE(fiber_suspend) {
  // switching to a nil fiber tells the interpreter to stop and exit
  vm.set_fiber(nullptr);
  return PrimitiveResult::FIBER;
}

DEF_PRIMITIVE(fiber_transfer) {
  return run_fiber(vm, args[0].as_fiber(), args, false, false);
}

DEF_PRIMITIVE(fiber_transfer1) {
  return run_fiber(vm, args[0].as_fiber(), args, false, true);
}

DEF_PRIMITIVE(fiber_transfer_error) {
  PrimitiveResult result = run_fiber(vm, args[0].as_fiber(), args, false, true);
  if (result == PrimitiveResult::FIBER)
    vm.fiber()->set_error(args[1]);
  return result;
}

DEF_PRIMITIVE(fiber_try) {
  FiberObject* current = vm.fiber();
  FiberObject* tried = args[0].as_fiber();

  if (tried->empty_frame())
    RETURN_ERR("cannot try a finished fiber");
  if (tried->caller() != nullptr)
    RETURN_ERR("fiber has already benn called");
  vm.set_fiber(tried);

  // remeber who ran it
  vm.fiber()->set_caller(current);
  vm.fiber()->set_caller_is_trying(true);

  // if the fiber was yielded, make the yield call return nil
  if (vm.fiber()->stack_size() > 0)
    vm.fiber()->set_value(vm.fiber()->stack_size() - 1, nullptr);

  return PrimitiveResult::FIBER;
}

DEF_PRIMITIVE(fiber_yield) {
  FiberObject* current = vm.fiber();
  vm.set_fiber(current->caller());

  // unhook this fiber from the one that called it
  current->set_caller(nullptr);
  current->set_caller_is_trying(false);

  if (vm.fiber() != nullptr) {
    // make the caller's run method return nil
    vm.fiber()->set_value(vm.fiber()->stack_size() - 1, nullptr);
  }

  return PrimitiveResult::FIBER;
}

DEF_PRIMITIVE(fiber_yield1) {
  FiberObject* current = vm.fiber();
  vm.set_fiber(current->caller());

  // unhook this fiber from the one that called it
  current->set_caller(nullptr);
  current->set_caller_is_trying(false);

  // if we do not have any other pending fibers, jump all the way out of
  // the interpreter
  if (vm.fiber() != nullptr) {
    // make the caller's run method return the argument passed to yield
    vm.fiber()->set_value(vm.fiber()->stack_size() - 1, args[1]);

    // when we yielding fiber resumes, we will store the result of the yield
    // call in it's stack, since Fiber.yield(value) has two arguments (the
    // Fiber class and the value) and we only need one slot for the result,
    // discard the other slot now
    current->pop();
  }

  return PrimitiveResult::FIBER;
}

DEF_PRIMITIVE(numeric_atan2) {
  RETURN_VAL(std::atan2(args[0].as_numeric(), args[1].as_numeric()));
}

DEF_PRIMITIVE(numeric_fraction) {
  double dummy;
  RETURN_VAL(std::modf(args[0].as_numeric(), &dummy));
}

DEF_PRIMITIVE(numeric_isinfinity) {
  RETURN_VAL(std::isinf(args[0].as_numeric()));
}

DEF_PRIMITIVE(numeric_isinteger) {
  double value = args[0].as_numeric();
  if (std::isnan(value) || std::isinf(value))
    RETURN_VAL(false);
  RETURN_VAL(std::trunc(value) == value);
}

DEF_PRIMITIVE(numeric_isnan) {
  RETURN_VAL(std::isnan(args[0].as_numeric()));
}

DEF_PRIMITIVE(numeric_sign) {
  double num = args[0].as_numeric();
  if (num > 0)
    RETURN_VAL(1);
  else if (num < 0)
    RETURN_VAL(-1);
  else
    RETURN_VAL(0);
}

DEF_PRIMITIVE(numeric_tostring) {
  RETURN_VAL(StringObject::from_numeric(vm, args[0].as_numeric()));
}

DEF_PRIMITIVE(numeric_truncate) {
  double integer;
  std::modf(args[0].as_numeric(), &integer);
  RETURN_VAL(integer);
}

DEF_PRIMITIVE(numeric_fromstring) {
  if (!validate_string(vm, args[1], "Argument"))
    return PrimitiveResult::FIBER;

  StringObject* s = args[1].as_string();
  if (s->size() == 0)
    RETURN_VAL(nullptr);

  errno = 0;
  char* end;
  double number = std::strtod(s->cstr(), &end);
  // skip past any trailing whitespace
  while (*end != '\0' && std::isspace(*end))
    ++end;

  if (errno == ERANGE) {
    vm.fiber()->set_error(
        StringObject::make_string(vm, "numeric literal is too large"));
    return PrimitiveResult::FIBER;
  }

  // we must have consumed the entrie string, otherwise, it contains non-number
  // characters and we can't parse it
  if (end < s->cstr() + s->size())
    RETURN_VAL(nullptr);

  RETURN_VAL(number);
}

DEF_PRIMITIVE(numeric_pi) {
  RETURN_VAL(3.14159265358979323846);
}

#define DEF_NUMERIC_INFIX(name, op)\
DEF_PRIMITIVE(numeric_##name) {\
  if (!validate_numeric(vm, args[1], "Right operand"))\
    return PrimitiveResult::FIBER;\
  RETURN_VAL(args[0].as_numeric() op args[1].as_numeric());\
}
#define DEF_NUMERIC_BITWISE(name, op)\
DEF_PRIMITIVE(numeric_bit##name) {\
  if (!validate_numeric(vm, args[1], "Right operand"))\
    return PrimitiveResult::FIBER;\
  u32_t lhs = Xt::as_type<u32_t>(args[0].as_numeric());\
  u32_t rhs = Xt::as_type<u32_t>(args[1].as_numeric());\
  RETURN_VAL(lhs op rhs);\
}
#define DEF_NUMERIC_FN(name, fn)\
DEF_PRIMITIVE(numeric_##name) {\
  RETURN_VAL(fn(args[0].as_numeric()));\
}

DEF_NUMERIC_INFIX(add, +)
DEF_NUMERIC_INFIX(sub, -)
DEF_NUMERIC_INFIX(mul, *)
DEF_NUMERIC_INFIX(div, /)
DEF_NUMERIC_INFIX(gt, >)
DEF_NUMERIC_INFIX(ge, >=)
DEF_NUMERIC_INFIX(lt, <)
DEF_NUMERIC_INFIX(le, <=)

DEF_NUMERIC_BITWISE(and, &)
DEF_NUMERIC_BITWISE(or, |)
DEF_NUMERIC_BITWISE(xor, ^)
DEF_NUMERIC_BITWISE(lshift, <<)
DEF_NUMERIC_BITWISE(rshift, >>)

DEF_NUMERIC_FN(abs, std::fabs)
DEF_NUMERIC_FN(acos, std::acos)
DEF_NUMERIC_FN(asin, std::asin)
DEF_NUMERIC_FN(atan, std::atan)
DEF_NUMERIC_FN(ceil, std::ceil)
DEF_NUMERIC_FN(cos, std::cos)
DEF_NUMERIC_FN(floor, std::floor)
DEF_NUMERIC_FN(neg, -)
DEF_NUMERIC_FN(sin, std::sin)
DEF_NUMERIC_FN(sqrt, std::sqrt)
DEF_NUMERIC_FN(tan, std::tan)

DEF_PRIMITIVE(numeric_mod) {
  if (!validate_numeric(vm, args[1], "Right operand"))
    return PrimitiveResult::FIBER;

  RETURN_VAL(std::fmod(args[0].as_numeric(), args[1].as_numeric()));
}

DEF_PRIMITIVE(numeric_eq) {
  if (!args[1].is_numeric())
    RETURN_VAL(false);

  RETURN_VAL(args[0].as_numeric() == args[1].as_numeric());
}

DEF_PRIMITIVE(numeric_ne) {
  if (!args[1].is_numeric())
    RETURN_VAL(true);

  RETURN_VAL(args[0].as_numeric() != args[1].as_numeric());
}

DEF_PRIMITIVE(numeric_bitnot) {
  // bitwise operators always work on 32-bit unsigned int
  u32_t val = Xt::as_type<u32_t>(args[0].as_numeric());
  RETURN_VAL(~val);
}

DEF_PRIMITIVE(numeric_dotdot) {
  if (!validate_numeric(vm, args[1], "Right hand side of range"))
    return PrimitiveResult::FIBER;

  double from = args[0].as_numeric();
  double to = args[1].as_numeric();
  RETURN_VAL(RangeObject::make_range(vm, from, to, true));
}

DEF_PRIMITIVE(numeric_dotdotdot) {
  if (!validate_numeric(vm, args[1], "Right hand side of range"))
    return PrimitiveResult::FIBER;

  double from = args[0].as_numeric();
  double to = args[1].as_numeric();
  RETURN_VAL(RangeObject::make_range(vm, from, to, false));
}

DEF_PRIMITIVE(object_same) {
  RETURN_VAL(args[1] == args[2]);
}

DEF_PRIMITIVE(object_not) {
  RETURN_VAL(false);
}

DEF_PRIMITIVE(object_eq) {
  RETURN_VAL(args[0] == args[1]);
}

DEF_PRIMITIVE(object_ne) {
  RETURN_VAL(args[0] != args[1]);
}

DEF_PRIMITIVE(object_is) {
  if (!args[1].is_class())
    RETURN_ERR("right operand must be a class");

  ClassObject* cls = vm.get_class(args[0]);
  ClassObject* base_cls = args[1].as_class();

  // walk the superclass chain looking for the class
  for (; cls != nullptr; cls = cls->superclass()) {
    if (base_cls == cls)
      RETURN_VAL(true);
  }

  RETURN_VAL(false);
}

DEF_PRIMITIVE(object_tostring) {
  BaseObject* obj = args[0].as_object();
  RETURN_VAL(StringObject::format(vm, "instance of @", obj->cls()->name()));
}

DEF_PRIMITIVE(object_type) {
  RETURN_VAL(vm.get_class(args[0]));
}

DEF_PRIMITIVE(string_byteat) {
  StringObject* s = args[0].as_string();
  int index = validate_index(vm, args[1], s->size(), "Index");
  if (index == -1)
    return PrimitiveResult::FIBER;

  RETURN_VAL(Xt::as_type<u8_t>((*s)[index]));
}

DEF_PRIMITIVE(string_bytecount) {
  RETURN_VAL(args[0].as_string()->size());
}

DEF_PRIMITIVE(string_endswith) {
  if (!validate_string(vm, args[1], "Argument"))
    return PrimitiveResult::FIBER;

  StringObject* string = args[0].as_string();
  StringObject* search = args[1].as_string();

  // corner case, if the search string is longer than return false right away
  if (search->size() > string->size())
    RETURN_VAL(false);

  int r = memcmp(
      string->cstr() + string->size() - search->size(),
      search->cstr(), search->size());
  RETURN_VAL(r == 0);
}

DEF_PRIMITIVE(string_indexof) {
  if (!validate_string(vm, args[1], "Argument"))
    return PrimitiveResult::FIBER;

  StringObject* string = args[0].as_string();
  StringObject* search = args[1].as_string();

  RETURN_VAL(string->find(search));
}

DEF_PRIMITIVE(string_iterate) {
  StringObject* s = args[0].as_string();

  // if we are starting the interation, return the first index
  if (args[1].is_nil()) {
    if (s->size() == 0)
      RETURN_VAL(false);
    RETURN_VAL(0);
  }

  if (!validate_int(vm, args[1], "Iterator"))
    return PrimitiveResult::FIBER;

  if (args[1].as_numeric() < 0)
    RETURN_VAL(false);

  int index = Xt::as_type<int>(args[1].as_numeric());
  ++index;
  if (index >= s->size())
    RETURN_VAL(false);

  RETURN_VAL(index);
}

DEF_PRIMITIVE(string_iterbyte) {
  StringObject* s = args[0].as_string();

  // if we're starting the iteration, return the first index
  if (args[1].is_nil()) {
    if (s->size() == 0)
      RETURN_VAL(false);
    RETURN_VAL(0);
  }

  if (!validate_int(vm, args[1], "Iterator"))
    return PrimitiveResult::FIBER;

  if (args[1].as_numeric() < 0)
    RETURN_VAL(false);
  int index = Xt::as_type<int>(args[1].as_numeric());

  // advance to the next byte
  ++index;
  if (index >= s->size())
    RETURN_VAL(false);

  RETURN_VAL(index);
}

DEF_PRIMITIVE(string_itervalue) {
  StringObject* s = args[0].as_string();
  int index = validate_index(vm, args[1], s->size(), "Iterator");
  if (index == -1)
    return PrimitiveResult::FIBER;

  RETURN_VAL(StringObject::make_string(vm, (*s)[index]));
}

DEF_PRIMITIVE(string_startswith) {
  if (!validate_string(vm, args[1], "Argument"))
    return PrimitiveResult::FIBER;

  StringObject* string = args[0].as_string();
  StringObject* search = args[1].as_string();

  // corner case, if the search string is longer than return false right away
  if (search->size() > string->size())
    RETURN_VAL(false);

  RETURN_VAL(memcmp(string->cstr(), search->cstr(), search->size()) == 0);
}

DEF_PRIMITIVE(string_contains) {
  if (!validate_string(vm, args[1], "Argument"))
    return PrimitiveResult::FIBER;

  StringObject* orig = args[0].as_string();
  StringObject* subs = args[1].as_string();
  RETURN_VAL(orig->find(subs) != -1);
}

DEF_PRIMITIVE(string_add) {
  if (!validate_string(vm, args[1], "Right operand"))
    return PrimitiveResult::FIBER;

  RETURN_VAL(StringObject::format(vm, "@@", args[0], args[1]));
}

DEF_PRIMITIVE(string_subscript) {
  StringObject* s = args[0].as_string();

  if (args[1].is_numeric()) {
    int index = validate_index(vm, args[1], s->size(), "Subscript");
    if (index == -1)
      return PrimitiveResult::FIBER;

    RETURN_VAL(StringObject::make_string(vm, (*s)[index]));
  }

  if (!args[1].is_range())
    RETURN_ERR("subscript must be a numeric or a range");

  auto [start, step, count] = calculate_range(
      vm, args[1].as_range(), s->size());
  if (start == -1)
    return PrimitiveResult::FIBER;

  RETURN_VAL(StringObject::make_string_from_range(vm, s, start, count, step));
}

DEF_PRIMITIVE(string_tostring) {
  RETURN_VAL(args[0]);
}

static PrimitiveResult call_function(WrenVM& vm, Value* args, int argc) {
  FunctionObject* fn;
  if (args[0].is_closure())
    fn = args[0].as_closure()->fn();
  else
    fn = args[0].as_function();

  if (argc < fn->arity())
    RETURN_ERR("function expects more arguments");
  return PrimitiveResult::CALL;
}

DEF_PRIMITIVE(fn_new) {
  if (!validate_function(vm, args[1], "Argument"))
    return PrimitiveResult::FIBER;

  // the block argument is already a function, so just return it
  RETURN_VAL(args[1]);
}

DEF_PRIMITIVE(fn_arity) {
  RETURN_VAL(args[0].as_function()->arity());
}

DEF_PRIMITIVE_FN(fn_call, 0)
DEF_PRIMITIVE_FN(fn_call, 1)
DEF_PRIMITIVE_FN(fn_call, 2)
DEF_PRIMITIVE_FN(fn_call, 3)
DEF_PRIMITIVE_FN(fn_call, 4)
DEF_PRIMITIVE_FN(fn_call, 5)
DEF_PRIMITIVE_FN(fn_call, 6)
DEF_PRIMITIVE_FN(fn_call, 7)
DEF_PRIMITIVE_FN(fn_call, 8)
DEF_PRIMITIVE_FN(fn_call, 9)
DEF_PRIMITIVE_FN(fn_call, 10)
DEF_PRIMITIVE_FN(fn_call, 11)
DEF_PRIMITIVE_FN(fn_call, 12)
DEF_PRIMITIVE_FN(fn_call, 13)
DEF_PRIMITIVE_FN(fn_call, 14)
DEF_PRIMITIVE_FN(fn_call, 15)
DEF_PRIMITIVE_FN(fn_call, 16)

DEF_PRIMITIVE(fn_tostring) {
  RETURN_VAL(StringObject::make_string(vm, "<fn>"));
}

DEF_PRIMITIVE(list_new) {
  RETURN_VAL(ListObject::make_list(vm, 0));
}

DEF_PRIMITIVE(list_add) {
  args[0].as_list()->add_element(args[1]);
  RETURN_VAL(args[1]);
}

DEF_PRIMITIVE(list_clear) {
  args[0].as_list()->clear();
  RETURN_VAL(nullptr);
}

DEF_PRIMITIVE(list_len) {
  RETURN_VAL(args[0].as_list()->count());
}

DEF_PRIMITIVE(list_insert) {
  ListObject* list = args[0].as_list();
  int index = validate_index(vm, args[1], list->count() + 1, "Index");
  if (index == -1)
    return PrimitiveResult::FIBER;

  list->insert(index, args[2]);
  RETURN_VAL(args[2]);
}

DEF_PRIMITIVE(list_remove) {
  ListObject* list = args[0].as_list();
  int index = validate_index(vm, args[1], list->count(), "Index");
  if (index == -1)
    return PrimitiveResult::FIBER;

  RETURN_VAL(list->remove(index));
}

DEF_PRIMITIVE(list_iterate) {
  ListObject* list = args[0].as_list();

  // if we are starting the interation, return the first index
  if (args[1].is_nil()) {
    if (list->count() == 0)
      RETURN_VAL(false);
    RETURN_VAL(0);
  }

  if (!validate_int(vm, args[1], "Iterator"))
    return PrimitiveResult::FIBER;

  int index = Xt::as_type<int>(args[1].as_numeric());

  // stop if we are out out bounds
  if (index < 0 || index >= list->count() - 1)
    RETURN_VAL(false);

  RETURN_VAL(index + 1);
}

DEF_PRIMITIVE(list_itervalue) {
  ListObject* list = args[0].as_list();
  int index = validate_index(vm, args[1], list->count(), "Iterator");
  if (index == -1)
    return PrimitiveResult::FIBER;

  RETURN_VAL(list->get_element(index));
}

DEF_PRIMITIVE(list_subscript) {
  ListObject* list = args[0].as_list();

  if (args[1].is_numeric()) {
    int index = validate_index(vm, args[1], list->count(), "Subscript");
    if (index ==  -1)
      return PrimitiveResult::FIBER;

    RETURN_VAL(list->get_element(index));
  }

  if (!args[1].is_range())
    RETURN_ERR("Subscript must be a numeric or a range");

  auto [start, step, count] = calculate_range(
      vm, args[1].as_range(), list->count());
  if (start == -1)
    return PrimitiveResult::FIBER;

  ListObject* result = ListObject::make_list(vm, count);
  for (int i = 0; i < count; ++i)
    result->set_element(i, list->get_element(start + i * step));
  RETURN_VAL(result);
}

DEF_PRIMITIVE(list_subscript_setter) {
  ListObject* list = args[0].as_list();
  int index = validate_index(vm, args[1], list->count(), "Index");
  if (index == -1)
    return PrimitiveResult::FIBER;

  list->set_element(index, args[2]);
  RETURN_VAL(args[2]);
}

DEF_PRIMITIVE(range_from) {
  RETURN_VAL(args[0].as_range()->from());
}

DEF_PRIMITIVE(range_to) {
  RETURN_VAL(args[0].as_range()->to());
}

DEF_PRIMITIVE(range_min) {
  RangeObject* range = args[0].as_range();
  RETURN_VAL(std::fmin(range->from(), range->to()));
}

DEF_PRIMITIVE(range_max) {
  RangeObject* range = args[0].as_range();
  RETURN_VAL(std::fmax(range->from(), range->to()));
}

DEF_PRIMITIVE(range_isinclusive) {
  RETURN_VAL(args[0].as_range()->is_inclusive());
}

DEF_PRIMITIVE(range_iterate) {
  RangeObject* range = args[0].as_range();

  if (range->from() == range->to() && !range->is_inclusive())
    RETURN_VAL(false);
  if (args[1].is_nil())
    RETURN_VAL(range->from());
  if (!validate_numeric(vm, args[1], "Iterator"))
    return PrimitiveResult::FIBER;

  double iterator = args[1].as_numeric();
  // iterate towards [to] from [from]
  if (range->from() < range->to()) {
    ++iterator;
    if (iterator > range->to())
      RETURN_VAL(false);
  }
  else {
    --iterator;
    if (iterator < range->to())
      RETURN_VAL(false);
  }
  if (!range->is_inclusive() && iterator == range->to())
    RETURN_VAL(false);

  RETURN_VAL(iterator);
}

DEF_PRIMITIVE(range_itervalue) {
  RETURN_VAL(args[1]);
}

DEF_PRIMITIVE(range_tostring) {
  RangeObject* range = args[0].as_range();

  std::stringstream ss;
  ss << range->from() << (range->is_inclusive() ? ".." : "...") << range->to();
  RETURN_VAL(StringObject::make_string(vm, ss.str()));
}

DEF_PRIMITIVE(map_new) {
  RETURN_VAL(MapObject::make_map(vm));
}

DEF_PRIMITIVE(map_subscript) {
  if (!validate_key(vm, args[1]))
    return PrimitiveResult::FIBER;

  if (auto val = args[0].as_map()->get(args[1]); val)
    RETURN_VAL(*val);
  RETURN_VAL(nullptr);
}

DEF_PRIMITIVE(map_subscript_setter) {
  if (!validate_key(vm, args[1]))
    return PrimitiveResult::FIBER;

  args[0].as_map()->set(args[1], args[2]);
  RETURN_VAL(args[2]);
}

DEF_PRIMITIVE(map_clear) {
  args[0].as_map()->clear();
  RETURN_VAL(nullptr);
}

DEF_PRIMITIVE(map_contains) {
  if (!validate_key(vm, args[1]))
    return PrimitiveResult::FIBER;

  if (auto v = args[0].as_map()->get(args[1]); v)
    RETURN_VAL(true);
  RETURN_VAL(false);
}

DEF_PRIMITIVE(map_len) {
  RETURN_VAL(args[0].as_map()->count());
}

DEF_PRIMITIVE(map_remove) {
  if (!validate_key(vm, args[1]))
    return PrimitiveResult::FIBER;

  RETURN_VAL(args[0].as_map()->remove(args[1]));
}

DEF_PRIMITIVE(map_iterate) {
  MapObject* map = args[0].as_map();

  if (map->count() == 0)
    RETURN_VAL(false);

  // if we are starting the iteration, start at the first used entry
  int index = 0;

  // otherwise, start one past the last entry we stopped at
  if (!args[1].is_nil()) {
    if (!validate_int(vm, args[1], "Iterator"))
      return PrimitiveResult::FIBER;

    if (args[1].as_numeric() < 0)
      RETURN_VAL(false);

    index = Xt::as_type<int>(args[1].as_numeric());
    if (index >= map->capacity())
      RETURN_VAL(false);

    // advance the iterator
    ++index;
  }

  // find a used entry if any
  for (; index < map->capacity(); ++index) {
    if (!(*map)[index].first.is_undefined())
      RETURN_VAL(index);
  }

  // if we get here, walked all of the entries
  RETURN_VAL(false);
}

DEF_PRIMITIVE(map_iterkey) {
  MapObject* map = args[0].as_map();
  int index = validate_index(vm, args[1], map->capacity(), "Iterator");
  if (index == -1)
    return PrimitiveResult::FIBER;

  const auto& entry = (*map)[index];
  if (entry.first.is_undefined())
    RETURN_ERR("invalid map iterator value");
  RETURN_VAL(entry.first);
}

DEF_PRIMITIVE(map_itervalue) {
  MapObject* map = args[0].as_map();
  int index = validate_index(vm, args[1], map->capacity(), "Iterator");
  if (index == -1)
    return PrimitiveResult::FIBER;

  const auto& entry = (*map)[index];
  if (entry.first.is_undefined())
    RETURN_ERR("invalid map iterator value");
  RETURN_VAL(entry.second);
}

static ClassObject* define_class(
    WrenVM& vm, ModuleObject* module, const str_t& name) {
  // creates either the Object or Class class in the core library with [name]

  StringObject* name_string = StringObject::make_string(vm, name);
  PinnedGuard guard(vm, name_string);

  ClassObject* cls = ClassObject::make_single_class(vm, name_string);
  vm.define_variable(module, name, cls);
  return cls;
}

namespace core {
  void initialize(WrenVM& vm) {
    ModuleObject* core_module = vm.get_core_module();

    // define the root object class, this has to be done a little specially
    // because it has no superclass
    vm.set_obj_cls(define_class(vm, core_module, "Object"));
    vm.set_primitive(vm.obj_cls(), "!", _primitive_object_not);
    vm.set_primitive(vm.obj_cls(), "==(_)", _primitive_object_eq);
    vm.set_primitive(vm.obj_cls(), "!=(_)", _primitive_object_ne);
    vm.set_primitive(vm.obj_cls(), "is(_)", _primitive_object_is);
    vm.set_primitive(vm.obj_cls(), "toString", _primitive_object_tostring);
    vm.set_primitive(vm.obj_cls(), "type", _primitive_object_type);

    // now we can define Class, which is a subclass of Object
    vm.set_class_cls(define_class(vm, core_module, "Class"));
    vm.class_cls()->bind_superclass(vm.obj_cls());
    vm.set_primitive(vm.class_cls(), "name", _primitive_class_name);
    vm.set_primitive(vm.class_cls(), "supertype", _primitive_class_supertype);
    vm.set_primitive(vm.class_cls(), "toString", _primitive_class_tostring);

    // finally we can define Object's metaclass which is a subclass of Class
    ClassObject* obj_metaclass = define_class(vm, core_module, "Object metaclass");
    // wire up the metaclass relationships now that all three classes are built
    vm.obj_cls()->set_cls(obj_metaclass);
    obj_metaclass->set_cls(vm.class_cls());
    vm.class_cls()->set_cls(vm.class_cls());

    // do this after wiring up the metaclasses so obj_metaclass doesn't get
    // collected
    obj_metaclass->bind_superclass(vm.class_cls());
    vm.set_primitive(obj_metaclass, "same(_,_)", _primitive_object_same);

    // the rest of the classes can now be defined normally
    vm.interpret("", kLibSource);

    vm.set_bool_cls(vm.find_variable(core_module, "Bool").as_class());
    vm.set_primitive(vm.bool_cls(), "toString", _primitive_bool_tostring);
    vm.set_primitive(vm.bool_cls(), "!", _primitive_bool_not);

    vm.set_fiber_cls(vm.find_variable(core_module, "Fiber").as_class());
    vm.set_primitive(vm.fiber_cls()->cls(), "new(_)", _primitive_fiber_new);
    vm.set_primitive(vm.fiber_cls()->cls(), "abort(_)", _primitive_fiber_abort);
    vm.set_primitive(vm.fiber_cls()->cls(), "current", _primitive_fiber_current);
    vm.set_primitive(vm.fiber_cls()->cls(), "suspend()", _primitive_fiber_suspend);
    vm.set_primitive(vm.fiber_cls()->cls(), "yield()", _primitive_fiber_yield);
    vm.set_primitive(vm.fiber_cls()->cls(), "yield(_)", _primitive_fiber_yield1);
    vm.set_primitive(vm.fiber_cls(), "call()", _primitive_fiber_call);
    vm.set_primitive(vm.fiber_cls(), "call(_)", _primitive_fiber_call1);
    vm.set_primitive(vm.fiber_cls(), "error", _primitive_fiber_error);
    vm.set_primitive(vm.fiber_cls(), "isDone", _primitive_fiber_isdone);
    vm.set_primitive(vm.fiber_cls(), "transfer()", _primitive_fiber_transfer);
    vm.set_primitive(vm.fiber_cls(), "transfer(_)", _primitive_fiber_transfer1);
    vm.set_primitive(vm.fiber_cls(), "transferError(_)", _primitive_fiber_transfer_error);
    vm.set_primitive(vm.fiber_cls(), "try()", _primitive_fiber_try);

    vm.set_fn_cls(vm.find_variable(core_module, "Function").as_class());
    vm.set_primitive(vm.fn_cls()->cls(), "new(_)", _primitive_fn_new);
    vm.set_primitive(vm.fn_cls(), "arity", _primitive_fn_arity);
    vm.set_primitive(vm.fn_cls(), "call()", _primitive_fn_call0);
    vm.set_primitive(vm.fn_cls(), "call(_)", _primitive_fn_call1);
    vm.set_primitive(vm.fn_cls(), "call(_,_)", _primitive_fn_call2);
    vm.set_primitive(vm.fn_cls(), "call(_,_,_)", _primitive_fn_call3);
    vm.set_primitive(vm.fn_cls(), "call(_,_,_,_)", _primitive_fn_call4);
    vm.set_primitive(vm.fn_cls(), "call(_,_,_,_,_)", _primitive_fn_call5);
    vm.set_primitive(vm.fn_cls(), "call(_,_,_,_,_,_)", _primitive_fn_call6);
    vm.set_primitive(vm.fn_cls(), "call(_,_,_,_,_,_,_)", _primitive_fn_call7);
    vm.set_primitive(vm.fn_cls(), "call(_,_,_,_,_,_,_,_)", _primitive_fn_call8);
    vm.set_primitive(vm.fn_cls(), "call(_,_,_,_,_,_,_,_,_)", _primitive_fn_call9);
    vm.set_primitive(vm.fn_cls(), "call(_,_,_,_,_,_,_,_,_,_)", _primitive_fn_call10);
    vm.set_primitive(vm.fn_cls(), "call(_,_,_,_,_,_,_,_,_,_,_)", _primitive_fn_call11);
    vm.set_primitive(vm.fn_cls(), "call(_,_,_,_,_,_,_,_,_,_,_,_)", _primitive_fn_call12);
    vm.set_primitive(vm.fn_cls(), "call(_,_,_,_,_,_,_,_,_,_,_,_,_)", _primitive_fn_call13);
    vm.set_primitive(vm.fn_cls(), "call(_,_,_,_,_,_,_,_,_,_,_,_,_,_)", _primitive_fn_call14);
    vm.set_primitive(vm.fn_cls(), "call(_,_,_,_,_,_,_,_,_,_,_,_,_,_,_)", _primitive_fn_call15);
    vm.set_primitive(vm.fn_cls(), "call(_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_)", _primitive_fn_call16);
    vm.set_primitive(vm.fn_cls(), "toString", _primitive_fn_tostring);

    vm.set_nil_cls(vm.find_variable(core_module, "Nil").as_class());
    vm.set_primitive(vm.nil_cls(), "!", _primitive_nil_not);
    vm.set_primitive(vm.nil_cls(), "toString", _primitive_nil_tostring);

    vm.set_num_cls(vm.find_variable(core_module, "Numeric").as_class());
    vm.set_primitive(vm.num_cls()->cls(), "fromString(_)", _primitive_numeric_fromstring);
    vm.set_primitive(vm.num_cls()->cls(), "pi", _primitive_numeric_pi);
    vm.set_primitive(vm.num_cls(), "+(_)", _primitive_numeric_add);
    vm.set_primitive(vm.num_cls(), "-(_)", _primitive_numeric_sub);
    vm.set_primitive(vm.num_cls(), "*(_)", _primitive_numeric_mul);
    vm.set_primitive(vm.num_cls(), "/(_)", _primitive_numeric_div);
    vm.set_primitive(vm.num_cls(), ">(_)", _primitive_numeric_gt);
    vm.set_primitive(vm.num_cls(), ">=(_)", _primitive_numeric_ge);
    vm.set_primitive(vm.num_cls(), "<(_)", _primitive_numeric_lt);
    vm.set_primitive(vm.num_cls(), "<=(_)", _primitive_numeric_le);
    vm.set_primitive(vm.num_cls(), "&(_)", _primitive_numeric_bitand);
    vm.set_primitive(vm.num_cls(), "|(_)", _primitive_numeric_bitor);
    vm.set_primitive(vm.num_cls(), "^(_)", _primitive_numeric_bitxor);
    vm.set_primitive(vm.num_cls(), "<<(_)", _primitive_numeric_bitlshift);
    vm.set_primitive(vm.num_cls(), ">>(_)", _primitive_numeric_bitrshift);
    vm.set_primitive(vm.num_cls(), "abs", _primitive_numeric_abs);
    vm.set_primitive(vm.num_cls(), "acos", _primitive_numeric_acos);
    vm.set_primitive(vm.num_cls(), "asin", _primitive_numeric_asin);
    vm.set_primitive(vm.num_cls(), "atan", _primitive_numeric_atan);
    vm.set_primitive(vm.num_cls(), "ceil", _primitive_numeric_ceil);
    vm.set_primitive(vm.num_cls(), "cos", _primitive_numeric_cos);
    vm.set_primitive(vm.num_cls(), "floor", _primitive_numeric_floor);
    vm.set_primitive(vm.num_cls(), "-", _primitive_numeric_neg);
    vm.set_primitive(vm.num_cls(), "sin", _primitive_numeric_sin);
    vm.set_primitive(vm.num_cls(), "sqrt", _primitive_numeric_sqrt);
    vm.set_primitive(vm.num_cls(), "tan", _primitive_numeric_tan);
    vm.set_primitive(vm.num_cls(), "%(_)", _primitive_numeric_mod);
    vm.set_primitive(vm.num_cls(), "~", _primitive_numeric_bitnot);
    vm.set_primitive(vm.num_cls(), "..(_)", _primitive_numeric_dotdot);
    vm.set_primitive(vm.num_cls(), "...(_)", _primitive_numeric_dotdotdot);
    vm.set_primitive(vm.num_cls(), "atan(_)", _primitive_numeric_atan2);
    vm.set_primitive(vm.num_cls(), "fraction", _primitive_numeric_fraction);
    vm.set_primitive(vm.num_cls(), "isInfinity", _primitive_numeric_isinfinity);
    vm.set_primitive(vm.num_cls(), "isInteger", _primitive_numeric_isinteger);
    vm.set_primitive(vm.num_cls(), "isNan", _primitive_numeric_isnan);
    vm.set_primitive(vm.num_cls(), "sign", _primitive_numeric_sign);
    vm.set_primitive(vm.num_cls(), "toString", _primitive_numeric_tostring);
    vm.set_primitive(vm.num_cls(), "truncate", _primitive_numeric_truncate);

    // these are defined just so that 0 and -0 are equal, which is specified
    // by IEEE-754 even though they have different bit representations
    vm.set_primitive(vm.num_cls(), "==(_)", _primitive_numeric_eq);
    vm.set_primitive(vm.num_cls(), "!=(_)", _primitive_numeric_ne);

    vm.set_str_cls(vm.find_variable(core_module, "String").as_class());
    vm.set_primitive(vm.str_cls(), "+(_)", _primitive_string_add);
    vm.set_primitive(vm.str_cls(), "[_]", _primitive_string_subscript);
    vm.set_primitive(vm.str_cls(), "byteAt(_)", _primitive_string_byteat);
    vm.set_primitive(vm.str_cls(), "byteCount", _primitive_string_bytecount);
    vm.set_primitive(vm.str_cls(), "contains(_)", _primitive_string_contains);
    vm.set_primitive(vm.str_cls(), "endsWith(_)", _primitive_string_endswith);
    vm.set_primitive(vm.str_cls(), "indexOf(_)", _primitive_string_indexof);
    vm.set_primitive(vm.str_cls(), "iterate(_)", _primitive_string_iterate);
    vm.set_primitive(vm.str_cls(), "iterByte(_)", _primitive_string_iterbyte);
    vm.set_primitive(vm.str_cls(), "iterValue(_)", _primitive_string_itervalue);
    vm.set_primitive(vm.str_cls(), "startsWith(_)", _primitive_string_startswith);
    vm.set_primitive(vm.str_cls(), "toString", _primitive_string_tostring);

    vm.set_list_cls(vm.find_variable(core_module, "List").as_class());
    vm.set_primitive(vm.list_cls()->cls(), "new()", _primitive_list_new);
    vm.set_primitive(vm.list_cls(), "[_]", _primitive_list_subscript);
    vm.set_primitive(vm.list_cls(), "[_]=(_)", _primitive_list_subscript_setter);
    vm.set_primitive(vm.list_cls(), "add(_)", _primitive_list_add);
    vm.set_primitive(vm.list_cls(), "clear()", _primitive_list_clear);
    vm.set_primitive(vm.list_cls(), "len", _primitive_list_len);
    vm.set_primitive(vm.list_cls(), "insert(_,_)", _primitive_list_insert);
    vm.set_primitive(vm.list_cls(), "remove(_)", _primitive_list_remove);
    vm.set_primitive(vm.list_cls(), "iterate(_)", _primitive_list_iterate);
    vm.set_primitive(vm.list_cls(), "iterValue(_)", _primitive_list_itervalue);

    vm.set_range_cls(vm.find_variable(core_module, "Range").as_class());
    vm.set_primitive(vm.range_cls(), "from", _primitive_range_from);
    vm.set_primitive(vm.range_cls(), "to", _primitive_range_to);
    vm.set_primitive(vm.range_cls(), "min", _primitive_range_min);
    vm.set_primitive(vm.range_cls(), "max", _primitive_range_max);
    vm.set_primitive(vm.range_cls(), "isInclusive", _primitive_range_isinclusive);
    vm.set_primitive(vm.range_cls(), "iterate(_)", _primitive_range_iterate);
    vm.set_primitive(vm.range_cls(), "iterValue(_)", _primitive_range_itervalue);
    vm.set_primitive(vm.range_cls(), "toString", _primitive_range_tostring);

    vm.set_map_cls(vm.find_variable(core_module, "Map").as_class());
    vm.set_primitive(vm.map_cls()->cls(), "new()", _primitive_map_new);
    vm.set_primitive(vm.map_cls(), "[_]", _primitive_map_subscript);
    vm.set_primitive(vm.map_cls(), "[_]=(_)", _primitive_map_subscript_setter);
    vm.set_primitive(vm.map_cls(), "clear()", _primitive_map_clear);
    vm.set_primitive(vm.map_cls(), "containsKey(_)", _primitive_map_contains);
    vm.set_primitive(vm.map_cls(), "len", _primitive_map_len);
    vm.set_primitive(vm.map_cls(), "remove(_)", _primitive_map_remove);
    vm.set_primitive(vm.map_cls(), "iter(_)", _primitive_map_iterate);
    vm.set_primitive(vm.map_cls(), "keyIterValue(_)", _primitive_map_iterkey);
    vm.set_primitive(vm.map_cls(), "valIterValue(_)", _primitive_map_itervalue);

    // while bootstrapping the core types and running the core library, a number
    // string objects have benn created, many of which were instantiated before
    // `str_class_` was stored in the VM, some of them *must* be created first:
    // the ClassObject from string itself has a reference to the StringObject
    // for its name.
    //
    // these all currently a `nullptr` `class_obj_` pointer, so go back and
    // assign them now that the string class is known
    vm.set_metaclasses();
  }
}

}
