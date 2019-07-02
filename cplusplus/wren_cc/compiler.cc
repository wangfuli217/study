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
#include <cstdarg>
#include <functional>
#include <iostream>
#include <vector>
#include "lexer.hh"
#include "value.hh"
#include "vm.hh"
#include "compiler.hh"

namespace wrencc {

class Compiler;
using GrammerFn = std::function<void (Compiler*, bool)>;
using SignatureFn = std::function<void (Compiler*, str_t&)>;

enum class Precedence {
  NONE,
  LOWEST,

  ASSIGNMENT, // =
  LOGIC,      // && ||
  IS,         // is
  EQUALITY,   // == !=
  COMPARISON, // < <= > >=
  BITWISE,    // | &
  TERM,       // + -
  FACTOR,     // * / %
  UNARY,      // unary - ! ~
  CALL,       // () []
};

inline Precedence operator+(Precedence a, int b) {
  return Xt::as_type<Precedence>(Xt::as_type<int>(a) + b);
}

struct GrammerRule {
  GrammerFn prefix;
  GrammerFn infix;
  SignatureFn method;
  Precedence precedence;
  const char* name;
};

class Parser : private UnCopyable {
  WrenVM& vm_;

  Lexer& lex_;
  Token prev_;
  Token curr_;

  bool skip_newlines_{true};

  bool had_error_{};
public:
  Parser(WrenVM& vm, Lexer& lex) noexcept
    : vm_(vm), lex_(lex) {
  }

  inline const Token& prev(void) const { return prev_; }
  inline const Token& curr(void) const { return curr_; }
  inline bool had_error(void) const { return had_error_; }
  inline void set_error(bool b = true) { had_error_ = b; }
  inline WrenVM& get_vm(void) { return vm_; }

  void advance(void) {
    prev_ = curr_;

    // if we are out of tokens, donot try to tokenize any more we *do*
    // skill copy the TokenKind::TK_EOF to previois so that so that code
    // that expects it to be consumed will still work
    if (curr_.kind() == TokenKind::TK_EOF)
      return;

    for (;;) {
      curr_ = lex_.next_token();
      switch (curr_.kind()) {
      case TokenKind::TK_NL:
        if (!skip_newlines_) {
          skip_newlines_ = true;
          return;
        }
        break;
      case TokenKind::TK_LPAREN:
      case TokenKind::TK_LBRACKET:
      case TokenKind::TK_LBRACE:
      case TokenKind::TK_DOT:
      case TokenKind::TK_COMMA:
      case TokenKind::TK_STAR:
      case TokenKind::TK_SLASH:
      case TokenKind::TK_PERCENT:
      case TokenKind::TK_PLUS:
      case TokenKind::TK_MINUS:
      case TokenKind::TK_PIPE:
      case TokenKind::TK_PIPEPIPE:
      case TokenKind::TK_AMP:
      case TokenKind::TK_AMPAMP:
      case TokenKind::TK_BANG:
      case TokenKind::TK_EQ:
      case TokenKind::TK_LT:
      case TokenKind::TK_GT:
      case TokenKind::TK_LTEQ:
      case TokenKind::TK_GTEQ:
      case TokenKind::TK_EQEQ:
      case TokenKind::TK_BANGEQ:
      case TokenKind::KW_CLASS:
      case TokenKind::KW_ELSE:
      case TokenKind::KW_IF:
      case TokenKind::KW_IS:
      case TokenKind::KW_STATIC:
      case TokenKind::KW_VAR:
      case TokenKind::KW_WHILE:
        skip_newlines_ = true; return;
      default: skip_newlines_ = false; return;
      }
    }
  }
};

struct Scope {
  // the number of previously defined local variables when this scope was
  // created. used to know how many variables to discard when this scope is
  // exited
  int prev_locals{};

  // the scope enclosing this one, or nullptr if this is the top scope in
  // the function
  Scope* parent{};

  void set_scope(int _locals_count = 0, Scope* _parent = nullptr) {
    prev_locals = _locals_count;
    parent = _parent;
  }
};

struct Local {
  str_t name{}; // the name of the local variable

  // the depth in the scope chain that this variable was declared at. Zero
  // is the outermost scope-parameters for a method, or the first local block
  // in top level code. one is the scope within that, etc.
  int depth{};

  Local(const str_t& n = str_t(), int d = -1) noexcept
    : name(n), depth(d) {
  }
};

class Compiler : private UnCopyable {
  // the maximum number of arguments that can be passed to a method, note
  // that this limtation is hardcoded in other places in th VM, in particular
  // the `CALL_xx` instructions assume a certain maximum number
  static constexpr int kMaxArguments = 16;

  // the maximum number of local variables that can be declared in a single
  // function, method, or check of top level code. this is the maximum number
  // of variables in scope at one time, and spans block scopes.
  //
  // note that this limitation is also explicit in the bytecode, since [LOAD_LOCAL]
  // and [STORE_LOCAL] use a single argument byte to identify the local, only
  // 256 can be in scope at one time.
  static constexpr int kMaxLocals = 256;

  Parser& parser_;
  Compiler* parent_{};
  FunctionObject* fn_{};
  int num_codes_{};

  // for the fields of the nearest enclosing class, or nullptr if not
  // currently inside a class
  SymbolTable* fields_{};

  bool is_method_{}; // true if the function being compiled is a method

  // the current level of block scope nesting, where zero is no nesting.
  // a -1 here means top-level code is being compiled and thers is no block
  // scope in effect at all. any variables declared will be global
  int scope_depth_{-1};

  // the currently in scope local variables
  std::vector<Local> locals_;

  inline SymbolTable& vm_methods(void) { return parser_.get_vm().methods(); }
  inline SymbolTable& vm_gsymbols(void) { return parser_.get_vm().gsymbols(); }

  void error(const char* format, ...) {
    parser_.set_error(true);
    std::cerr << "[LINE:" << parser_.prev().lineno() << "] - "
      << "Compile ERROR on `" << parser_.prev().literal() << "` : ";

    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
    std::cerr << std::endl;
  }

  int intern_symbol(void) {
    return vm_methods().ensure(parser_.prev().as_string());
  }

  template <typename T> inline int emit_byte(T b) {
    return fn_->add_code(b);
  }

  template <typename T, typename U> inline void emit_bytes(T b1, U b2) {
    emit_byte(b1);
    emit_byte(b2);
  }

  inline void patch_jump(int offset) {
    // replaces the placeholder argument for a previous JUMP or JUMP_IF
    // instruction with an offset that jumps to the current end of bytecode

    fn_->set_code(offset, fn_->codes_count() - offset - 1);
  }

  inline int add_constant(const Value& v) {
    return fn_->add_constant(v);
  }

  inline void emit_constant(const Value& v) {
    u8_t b = fn_->add_constant(v);
    emit_bytes(Code::CONSTANT, b);
  }

  void push_scope(void) {
    // starts a new local block scope

    ++scope_depth_;
  }

  void pop_scope(void) {
    // closes the last pushed block scope

    ASSERT(scope_depth_ > -1, "cannot pop top-level scope");

    // pop locals off the stack
    while (!locals_.empty() && locals_.back().depth == scope_depth_) {
      locals_.pop_back();
      emit_byte(Code::POP);
    }
    --scope_depth_;
  }

  const GrammerRule& get_rule(TokenKind kind) const {
#define RULE(fn) [](Compiler* c, bool b) { c->fn(b); }
#define SIGN(fn) [](Compiler* c, str_t& n) { c->fn(n); }

#define UNUSED {nullptr, nullptr, nullptr, Precedence::NONE, nullptr}
#define PREFIX(fn) {RULE(fn), nullptr, nullptr, Precedence::NONE, nullptr}
#define INFIX(fn, prec) {nullptr, RULE(fn), nullptr, prec, nullptr}
#define INFIXOP(prec, name) {nullptr, RULE(infix_oper), SIGN(infix_signature), prec, name}
#define OPER(prec, name) {RULE(unary_oper), RULE(infix_oper), SIGN(mixed_signature), prec, name}
#define OPER2(pfn, ifn, prec) {RULE(pfn), RULE(ifn), nullptr, prec, nullptr}
#define PREFIXOP(name) {RULE(unary_oper), nullptr, SIGN(unary_signature), Precedence::NONE, name}
#define PREFIXNAME {RULE(variable), nullptr, SIGN(parameters), Precedence::NONE, nullptr}
    static const GrammerRule _rules[] = {
      PREFIX(grouping),                         // PUNCTUATOR(LPAREN, "(")
      UNUSED,                                   // PUNCTUATOR(RPAREN, ")")
      OPER2(list, subscript, Precedence::CALL), // PUNCTUATOR(LBRACKET, "[")
      UNUSED,                                   // PUNCTUATOR(RBRACKET, "]")
      UNUSED,                                   // PUNCTUATOR(LBRACE, "{")
      UNUSED,                                   // PUNCTUATOR(RBRACE, "}")
      UNUSED,                                   // PUNCTUATOR(COLON, ":")
      INFIX(call, Precedence::CALL),            // PUNCTUATOR(DOT, ".")
      UNUSED,                                   // PUNCTUATOR(COMMA, ",")
      INFIXOP(Precedence::FACTOR, "* "),        // PUNCTUATOR(STAR, "*")
      INFIXOP(Precedence::FACTOR, "/ "),        // PUNCTUATOR(SLASH, "/")
      INFIXOP(Precedence::TERM, "% "),          // PUNCTUATOR(PERCENT, "%")
      INFIXOP(Precedence::TERM, "+ "),          // PUNCTUATOR(PLUS, "+")
      OPER(Precedence::TERM, "- "),             // PUNCTUATOR(MINUS, "-")
      UNUSED,                                   // PUNCTUATOR(PIPE, "|")
      INFIX(or_expr, Precedence::LOGIC),        // PUNCTUATOR(PIPEPIPE, "||")
      UNUSED,                                   // PUNCTUATOR(AMP, "&")
      INFIX(and_expr, Precedence::LOGIC),       // PUNCTUATOR(AMPAMP, "&&")
      PREFIXOP("!"),                            // PUNCTUATOR(BANG, "!")
      UNUSED,                                   // PUNCTUATOR(EQ, "=")
      INFIXOP(Precedence::COMPARISON, "< "),    // PUNCTUATOR(LT, "<")
      INFIXOP(Precedence::COMPARISON, "> "),    // PUNCTUATOR(GT, ">")
      INFIXOP(Precedence::COMPARISON, "<= "),   // PUNCTUATOR(LTEQ, "<=")
      INFIXOP(Precedence::COMPARISON, ">= "),   // PUNCTUATOR(GTEQ, ">=")
      INFIXOP(Precedence::EQUALITY, "== "),     // PUNCTUATOR(EQEQ, "==")
      INFIXOP(Precedence::EQUALITY, "!= "),     // PUNCTUATOR(BANGEQ, "!=")

      UNUSED,                                   // KEYWORD(CLASS, "class")
      UNUSED,                                   // KEYWORD(ELSE, "else")
      PREFIX(boolean),                          // KEYWORD(FALSE, "false")
      PREFIX(function),                         // KEYWORD(FN, "fn")
      UNUSED,                                   // KEYWORD(IF, "if")
      INFIX(is, Precedence::IS),                // KEYWORD(IS, "is")
      PREFIX(nil),                              // KEYWORD(NIL, "nil")
      UNUSED,                                   // KEYWORD(STATIC, "static")
      PREFIX(this_exp),                         // KEYWORD(THIS, "this")
      PREFIX(boolean),                          // KEYWORD(TRUE, "true")
      UNUSED,                                   // KEYWORD(VAR, "var")
      UNUSED,                                   // KEYWORD(WHILE, "while")

      PREFIX(field),                            // TOKEN(FIELD, "field")
      PREFIXNAME,                               // TOKEN(IDENTIFIER, "identifier")
      PREFIX(numeric),                          // TOKEN(NUMERIC, "numeric")
      PREFIX(string),                           // TOKEN(STRING, "string")

      UNUSED,                                   // TOKEN(NL, "new-line")

      UNUSED,                                   // TOKEN(ERROR, "error")
      UNUSED,                                   // TOKEN(EOF, "eof")
    };
#undef SIGN
#undef RULE

    return _rules[Xt::as_type<int>(kind)];
  }

  void parse_precedence(bool allow_assignment, Precedence precedence) {
    parser_.advance();
    auto& prefix_fn = get_rule(parser_.prev().kind()).prefix;
    if (!prefix_fn) {
      error("unexpected token for expression");
      return;
    }

    prefix_fn(this, allow_assignment);

    while (precedence <= get_rule(parser_.curr().kind()).precedence) {
      parser_.advance();

      auto& infix_fn = get_rule(parser_.prev().kind()).infix;
      if (infix_fn)
        infix_fn(this, allow_assignment);
    }
  }

  int declare_variable(void) {
    consume(TokenKind::TK_IDENTIFIER, "expected variable name");

    str_t name = parser_.prev().as_string();
    // top-level global scope
    if (scope_depth_ == -1) {
      int symbol = vm_gsymbols().add(name);
      if (symbol == -1)
        error("global variable is already defined");
      return symbol;
    }

    // see if there is already a variable with this name declared in the
    // current scope (outer scopes are OK: those get shadowed)
    int i = Xt::as_type<int>(locals_.size()) - 1;
    for (; i >= 0; --i) {
      auto& local = locals_[i];

      // once we escape this scope and hit an outer one, we can stop
      if (local.depth < scope_depth_)
        break;
      if (local.name == name) {
        error("variable is already declared in this scope");
        return i;
      }
    }

    if (locals_.size() >= kMaxLocals) {
      error("cannot declare more than %d variables in one scope", kMaxLocals);
      return -1;
    }

    // define a new local variable in the current scope
    locals_.push_back(Local(name, scope_depth_));
    return Xt::as_type<int>(locals_.size() - 1);
  }

  void define_variable(int symbol) {
    // handle top-level global scope
    if (scope_depth_ == -1) {
      // it's a global variable, so store the value int the global slot
      emit_bytes(Code::STORE_GLOBAL, symbol);
    }
    else {
      // it's a local variable, the value is already in the right slot to store
      // the local, but later code will pop and discard that. to cancel that out
      // duplicate it now, so that the temporary value will be discarded and
      // leave the local still on the stack.
      //
      //    var a = "value"
      //    io.write(a)
      //
      //    CONSTANT "value"    -> put constant into local slot
      //    DUP                 -> dup it so the top is a temporary
      //    POP                 -> discard previous result in sequence
      //    <code for io.write>
      //
      // would be good to either peephole optimize this or be smarted about
      // generating code for defining local variables to not emit the DUP
      // sometimes
      emit_byte(Code::DUP);
    }
  }

  int resolve_local(bool& is_global) {
    // look up the previously consumed token, which is presumed to be a
    // TK_IDENTIFIER in the currenr scope to see what identifier it's
    // bound to. returns the index of the identifier either global or local
    // scope. returns -1 if not found. sets [is_global] to `true` if the
    // identifier is in global scope, or `false` if in local.

    str_t name = parser_.prev().as_string();

    is_global = false;
    int i = Xt::as_type<int>(locals_.size()) - 1;
    for (; i >= 0; --i) {
      if (locals_[i].name == name)
        return i;
    }

    is_global = true;
    return vm_gsymbols().get(name);
  }

  void nil(bool allow_assignment) {
    emit_byte(Code::NIL);
  }

  void boolean(bool allow_assignment) {
    if (parser_.prev().kind() == TokenKind::KW_TRUE)
      emit_byte(Code::TRUE);
    else
      emit_byte(Code::FALSE);
  }

  void numeric(bool allow_assignment) {
    auto& tok = parser_.prev();
    emit_constant(tok.as_numeric());
  }

  void string(bool allow_assignment) {
    auto& tok = parser_.prev();
    emit_constant(StringObject::make_string(
          parser_.get_vm(), tok.as_string()));
  }

  void list(bool allow_assignment) {
    // compile the list elements
    int num_elements = 0;
    if (parser_.curr().kind() != TokenKind::TK_RBRACKET) {
      do {
        ++num_elements;
        assignment();
      } while (match(TokenKind::TK_COMMA));
    }
    consume(TokenKind::TK_RBRACKET, "expect `]` after list elements");

    // create the list
    emit_bytes(Code::LIST, num_elements);
  }

  void variable(bool allow_assignment) {
    // look up the name in the scope chain
    bool is_global;
    int index = resolve_local(is_global);
    if (index == -1)
      error("undefined variable");

    // if there's an `=` after a bare name, it's a variable assignment
    if (match(TokenKind::TK_EQ)) {
      if (!allow_assignment)
        error("invalid assignment");

      // compile the right-hand side
      statement();

      if (is_global)
        emit_bytes(Code::STORE_GLOBAL, index);
      else
        emit_bytes(Code::STORE_LOCAL, index);
      return;
    }

    // otherwise, it's just a variable access
    if (is_global)
      emit_bytes(Code::LOAD_GLOBAL, index);
    else
      emit_bytes(Code::LOAD_LOCAL, index);
  }

  void field(bool allow_assignment) {
    int field;
    if (fields_ != nullptr) {
      // look up the field, or implicitlt define it
      field = fields_->ensure(parser_.prev().as_string());
    }
    else {
      error("cannot reference a field outside of a class definition");
      // initialize it with a fake value so we can keep parsing and minimize
      // the number of cascaded erros
      field = 0xff;
    }

    // if there is an `=` after a filed name, it's an assignment
    if (match(TokenKind::TK_EQ)) {
      if (!allow_assignment)
        error("invalid assignment");

      // compile the right-hand side
      statement();

      emit_bytes(Code::STORE_FIELD, field);
    }
    else {
      emit_bytes(Code::LOAD_FIELD, field);
    }
  }

  bool match(TokenKind expected) {
    if (parser_.curr().kind() != expected)
      return false;

    parser_.advance();
    return true;
  }

  void consume(TokenKind expected, const char* msg) {
    parser_.advance();

    if (parser_.prev().kind() != expected)
      error(msg);
  }

  void definition(void) {
    if (match(TokenKind::KW_CLASS)) {
      // create a variable to store the class in
      int symbol = declare_variable();

      // load the superclass (if there is one)
      if (match(TokenKind::KW_IS)) {
        parse_precedence(false, Precedence::CALL);
        emit_byte(Code::SUBCLASS);
      }
      else {
        // create the empty class
        emit_byte(Code::CLASS);
      }

      // store a placeholder for the number of fields argument, we donot
      // know the value untial we have compiled all the methods to see
      // which fields are used.
      int num_fields_instruction = emit_byte(0xff);
      // store it in its name
      define_variable(symbol);
      // compile the method definition
      consume(TokenKind::TK_LBRACE, "expect `{` before class body");

      // set up a symbol table for the class's fields
      SymbolTable* prev_fields = fields_;
      SymbolTable fileds;
      fields_ = &fileds;

      while (!match(TokenKind::TK_RBRACE)) {
        Code instruction = Code::METHOD_INSTANCE;
        if (match(TokenKind::KW_STATIC)) {
          instruction = Code::METHOD_STATIC;
        }
        else if (match(TokenKind::KW_THIS)) {
          // if the method name is prefixed with `this` it's a named constructor
          instruction = Code::METHOD_CTOR;
        }

        auto& signature = get_rule(parser_.curr().kind()).method;
        parser_.advance();

        if (!signature) {
          error("expect method definition");
          break;
        }
        method(instruction, signature);
        consume(TokenKind::TK_NL, "expect newline after definition in class");
      }

      // update the class with the number of fields
      fn_->set_code(num_fields_instruction, fileds.count());
      fields_ = prev_fields;

      return;
    }

    if (match(TokenKind::KW_VAR)) {
      int symbol = declare_variable();
      consume(TokenKind::TK_EQ, "expect `=` after variable name");
      // compile the initializer
      statement();
      define_variable(symbol);
      return;
    }
    statement();
  }

  void statement(void) {
    if (match(TokenKind::KW_IF)) {
      // compile the condition
      consume(TokenKind::TK_LPAREN, "expect `(` after `if` keyword");
      assignment();
      consume(TokenKind::TK_RPAREN, "expect `)` after if condition");

      // jump to the else branch if the condition is false
      emit_byte(Code::JUMP_IF);
      int if_jump = emit_byte(0xff);

      // compile the then branch
      push_scope();
      definition();
      pop_scope();

      // jump over the else branch when the if branch is taken
      emit_byte(Code::JUMP);
      int else_jump = emit_byte(0xff);

      patch_jump(if_jump);
      // compile the else branch if thers is one
      if (match(TokenKind::KW_ELSE)) {
        push_scope();
        definition();
        pop_scope();
      }
      else {
        emit_byte(Code::NIL); // just default to nil
      }
      // patch the jump over the else
      patch_jump(else_jump);

      return;
    }

    if (match(TokenKind::KW_WHILE)) {
      // remember what instruction to loop back to
      int loop_start = fn_->codes_count() - 1;

      // compile the condition
      consume(TokenKind::TK_LPAREN, "expect `(` after while keyword");
      assignment();
      consume(TokenKind::TK_RPAREN, "expect `)` after while condition");

      emit_byte(Code::JUMP_IF);
      int exit_jump = emit_byte(0xff);

      // compile the while body
      push_scope();
      definition();
      pop_scope();

      // loop back to the while top
      emit_byte(Code::LOOP);
      int loop_offset = fn_->codes_count() - loop_start;
      emit_byte(loop_offset);

      patch_jump(exit_jump);

      // a while loop always evaluates to nil
      emit_byte(Code::NIL);
      return;
    }

    // curly block
    if (match(TokenKind::TK_LBRACE)) {
      push_scope();
      finish_block();
      pop_scope();
      return;
    }

    assignment();
  }

  void expression(bool allow_assignment) {
    parse_precedence(allow_assignment, Precedence::LOWEST);
  }

  void grouping(bool allow_assignment) {
    assignment();
    consume(TokenKind::TK_RPAREN, "expect `)` after expression");
  }

  void finish_block(void) {
    // parses a block body, after the initial `{` has been consumed

    for (;;) {
      definition();

      if (!match(TokenKind::TK_NL)) {
        consume(TokenKind::TK_RBRACE, "expect `}` after block body");
        break;
      }
      if (match(TokenKind::TK_RBRACE))
        break;

      // discard the result of the previous expression
      emit_byte(Code::POP);
    }
  }

  void function(bool allow_assignment) {
    Compiler fn_compiler(parser_, this, false);
    int fn_constant = fn_compiler.init_compiler();

    str_t dummy_name;
    fn_compiler.parameters(dummy_name);

    if (fn_compiler.match(TokenKind::TK_LBRACE))
      fn_compiler.finish_block();
    else
      fn_compiler.expression(false);

    fn_compiler.finish_compiler(fn_constant);
  }

  void method(Code instruction, const SignatureFn& signature) {
    // compiles a method definition inside a class body
    // consume(TokenKind::TK_IDENTIFIER, "expect method name");

    Compiler method_compiler(parser_, this, true);
    int method_constant = method_compiler.init_compiler();

    // build the method name, the mangled name includes all of the name parts
    // in a mixfix call as well as spaces for every argument.
    // so a method like:
    //    foo.bar(a, b) else (c) last
    //
    // will have name: "bar  else last"
    str_t name(parser_.prev().as_string());
    signature(&method_compiler, name);
    int symbol = vm_methods().ensure(name);

    consume(TokenKind::TK_LBRACE, "expect `{` to begin method body");
    method_compiler.finish_block();

    // if it's a constructor, return `this`, not the result of the body
    if (instruction == Code::METHOD_CTOR) {
      method_compiler.emit_byte(Code::POP);
      // the receiver is always stored in the first local slot
      method_compiler.emit_bytes(Code::LOAD_LOCAL, 0);
    }
    method_compiler.finish_compiler(method_constant);

    // compile the code to define the method it
    emit_byte(instruction);
    emit_byte(symbol);
  }

  void assignment(void) {
    // assignment statement
    expression(true);
  }

  void call(bool allow_assignment) {
    str_t name;
    int argc = 0;

    consume(TokenKind::TK_IDENTIFIER, "expect method name after `.`");
    name += parser_.prev().as_string();
    if (match(TokenKind::TK_LPAREN)) {
      do {
        // the VM can only handle a certain number of parameters, so check
        // for this explicitly and give a usable error
        if (++argc >= kMaxArguments + 1) {
          // only show an error at exactly `max + 1` and do not break so that
          // we can keep parsing the parameter list and minimize cascaded errors
          error("cannot pass more than %d arguments to a method", kMaxArguments);
        }

        statement();

        name.push_back(' ');
      } while (match(TokenKind::TK_COMMA));
      consume(TokenKind::TK_RPAREN, "expect `)` after arguments");
    }
    int symbol = vm_methods().ensure(name);

    // compile the method call
    emit_bytes(Code::CALL_0 + argc, symbol);
  }

  void subscript(bool allow_assignment) {
    // subscript or `array indexing` operator like `foo[index]`

    int argc = 0;
    // build the method name, allow overloading by arity, add a space to
    // the name for each argument
    str_t name(1, '[');

    // parse the arguments list
    do {
      // the VM can only handle a certain number of parameters, so check
      // for this explicitly and give a usable error
      if (++argc >= kMaxArguments + 1) {
        // only show an error at exactly `max + 1` and do not break so that
        // we can keep parsing the parameter list and minimize cascaded errors
        error("cannot pass more than %d arguments to a method", kMaxArguments);
      }

      statement();

      // add a space in the name for each argument, lets overload by arity
      name.push_back(' ');
    } while (match(TokenKind::TK_COMMA));
    consume(TokenKind::TK_RBRACKET, "expect `]` after subscript arguments");
    name.push_back(']');

    int symbol = vm_methods().ensure(name);
    // compile the method call
    emit_bytes(Code::CALL_0 + argc, symbol);
  }

  void is(bool allow_assignment) {
    // compile the right-hand side
    parse_precedence(false, Precedence::CALL);
    emit_byte(Code::IS);
  }

  void and_expr(bool allow_assignment) {
    // skip the right argument if the left is false

    emit_byte(Code::AND);
    int jump = emit_byte(0xff);

    parse_precedence(false, Precedence::LOGIC);
    patch_jump(jump);
  }

  void or_expr(bool allow_assignment) {
    // skip the right argument if the left if true

    emit_byte(Code::OR);
    int jump = emit_byte(0xff);

    parse_precedence(false, Precedence::LOGIC);
    patch_jump(jump);
  }

  void infix_oper(bool allow_assignment) {
    auto& rule = get_rule(parser_.prev().kind());

    // compile the right hand side
    parse_precedence(false, rule.precedence + 1);

    // call the operator method on the left-hand side
    int symbol = vm_methods().ensure(rule.name);
    emit_bytes(Code::CALL_1, symbol);
  }

  void unary_oper(bool allow_assignment) {
    auto& rule = get_rule(parser_.prev().kind());

    // compile the argument
    parse_precedence(false, Precedence::UNARY + 1);

    // call the operator method on the left-hand side
    int symbol = vm_methods().ensure(str_t(1, rule.name[0]));
    emit_bytes(Code::CALL_0, symbol);
  }

  void infix_signature(str_t& name) {
    // compiles a method signature for an infix operator

    // add a space for the RHS parameter
    name.push_back(' ');

    // parse the parameter name
    declare_variable();
  }

  void unary_signature(str_t& name) {
    // compiles a method signature for an unary operator
  }

  void mixed_signature(str_t& name) {
    // if there is a parameter name, it's an infix operator, otherwise it's unary
    if (parser_.curr().kind() == TokenKind::TK_IDENTIFIER) {
      // add a space for the RHS parameter
      name.push_back(' ');

      // parse the parameter name
      declare_variable();
    }
  }

  void parameters(str_t& name) {
    // parse the parameter list, if any
    if (match(TokenKind::TK_LPAREN)) {
      int argc = 0;
      do {
        // the VM can only handle a certain number of parameters, so
        // check for this explicitly and give a usable error
        if (++argc >= kMaxArguments + 1) {
          // only show an error at exactly `max + 1` and do not break
          // so that we can keep parsing the parameter list and minimize
          // cascaded errors
          error("cannot have more than %d parameters", kMaxArguments);
        }

        // define a local variable in the method for the parameters
        declare_variable();

        // add a space in the name for the parameters
        name.push_back(' ');
      } while (match(TokenKind::TK_COMMA));
      consume(TokenKind::TK_RPAREN, "expect `)` after parameters");
    }
  }

  void this_exp(bool allow_assignment) {
    // walk up the parent chain to see if there is an enclosing method
    Compiler* this_compiler = this;
    bool inside_method = false;
    while (this_compiler != nullptr) {
      if (this_compiler->is_method_) {
        inside_method = true;
        break;
      }
      this_compiler = this_compiler->parent_;
    }

    if (!inside_method) {
      error("cannot use `this` outside of a method");
      return;
    }

    // the receiver is always stored in the first local slot
    emit_bytes(Code::LOAD_LOCAL, 0);
  }
public:
  Compiler(Parser& parser,
      Compiler* parent = nullptr, bool is_method = false) noexcept
    : parser_(parser)
    , parent_(parent)
    , is_method_(is_method) {
  }

  ~Compiler(void) {
    // FIXME: fixed deallocate FunctionObject by GC
  }

  int init_compiler(void) {
    if (parent_ == nullptr) {
      scope_depth_ = -1;
    }
    else {
      locals_.push_back(Local());
      scope_depth_ = 0;
    }

    if (parent_ != nullptr)
      fields_ = parent_->fields_;
    fn_ = FunctionObject::make_function(parser_.get_vm());

    if (parent_ == nullptr)
      return -1;

    // add the block to the constant table, do this eagerly to it's
    // reachable by the GC
    return parent_->add_constant(fn_);
  }

  void finish_compiler(int constant) {
    // finishes [compiler], which is compiling a function, method or chunk of
    // top level code. if there is a parent compiler, then this emits code in
    // the parent compiler to loadd the resulting function

    emit_byte(Code::END); // end the function's code
    if (parent_ != nullptr) {
      // int the function that contains this one, load the resulting
      // function object
      emit_bytes(Code::CONSTANT, constant);
    }
  }

  FunctionObject* compile_function(TokenKind end_kind) {
    Pinned pinned;
    parser_.get_vm().pin_object(fn_, &pinned);
    for (;;) {
      definition();

      // if there is no newline, it must be the end of the block on the same line
      if (!match(TokenKind::TK_NL)) {
        consume(end_kind, "expect end of file");
        break;
      }
      if (match(end_kind))
        break;
      emit_byte(Code::POP);
    }
    finish_compiler(-1);

    parser_.get_vm().unpin_object();
    return parser_.had_error() ? nullptr : fn_;
  }

  FunctionObject* compile_function(
      Parser& p, Compiler* parent, TokenKind end_kind) {
    Compiler c(p, parent, false);
    return c.compile_function(end_kind);
  }
};

FunctionObject* compile(WrenVM& vm, const str_t& source_bytes) {
  Lexer lex(source_bytes);
  Parser p(vm, lex);

  p.advance();
  Compiler c(p, nullptr, false);
  c.init_compiler();

  return c.compile_function(TokenKind::TK_EOF);
}

}
