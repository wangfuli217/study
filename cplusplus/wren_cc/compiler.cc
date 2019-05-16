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
#include "vm.hh"
#include "compiler.hh"

namespace wrencc {

class Compiler;
using CompileFn = std::function<void (Compiler*, const Token&)>;

enum class Precedence {
  NONE,
  LOWEST,

  EQUALITY, // == !=
  COMPARISON, // < <= > >=
  BITWISE, // | &
  TERM, // + -
  FACTOR, // * / %
  CALL, // ()
};

struct InfixCompiler {
  CompileFn fn;
  Precedence precedence;
};

class Parser : private UnCopyable {
  VM& vm_;

  Lexer& lex_;
  Token prev_;
  Token curr_;

  bool had_error_{};
public:
  Parser(VM& vm, Lexer& lex) noexcept
    : vm_(vm), lex_(lex) {
  }

  inline const Token& prev(void) const { return prev_; }
  inline const Token& curr(void) const { return curr_; }
  inline bool had_error(void) const { return had_error_; }
  inline void set_error(bool b = true) { had_error_ = b; }
  inline VM& get_vm(void) { return vm_; }

  inline void advance(void) {
    prev_ = curr_;
    curr_ = lex_.next_token();
  }
};

class Compiler : private UnCopyable {
  Parser& parser_;
  Compiler* parent_{};
  BlockObject* block_{};
  int num_codes_{};

  SymbolTable locals_;

  void error(const char* format, ...) {
    parser_.set_error(true);
    std::cerr << "Compile ERROR on `" << parser_.prev().literal() << "` : ";

    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
    std::cerr << std::endl;
  }

  int intern_symbol(void) {
    return parser_.get_vm().symbols().ensure(parser_.prev().as_string());
  }

  template <typename T> inline void emit_byte(T b) {
    block_->add_code(b);
  }

  template <typename T, typename U> inline void emit_bytes(T b1, U b2) {
    emit_byte(b1);
    emit_byte(b2);
  }

  inline void emit_constant(Value v) {
    u8_t b = block_->add_constant(v);
    emit_bytes(Code::CONSTANT, b);
  }

  void numeric(void) {
    auto& tok = parser_.prev();
    Value constant = NumericObject::make_numeric(tok.as_numeric());

    emit_constant(constant);
  }

  bool match(TokenKind expected) {
    if (parser_.curr().kind() != expected)
      return false;

    parser_.advance();
    return true;
  }

  void consume(TokenKind expected) {
    parser_.advance();

    if (parser_.prev().kind() != expected)
      error("expected %d, got %d", expected, parser_.prev().kind());
  }

  void statement(void) {
    if (match(TokenKind::KW_CLASS)) {
      consume(TokenKind::TK_IDENTIFIER);

      int local = locals_.add(parser_.prev().as_string());
      if (local == -1)
        error("local variable is already defined");

      // create the empty class
      emit_byte(Code::CLASS);
      // store it in it's name
      emit_bytes(Code::STORE_LOCAL, local);

      // compile the method definitions
      consume(TokenKind::TK_LBRACE);
      while (!match(TokenKind::TK_RBRACE)) {
        // method name
        consume(TokenKind::TK_IDENTIFIER);
        consume(TokenKind::TK_LBRACE);
        // TODO: parse body
        consume(TokenKind::TK_RBRACE);

        consume(TokenKind::TK_NL);
      }
      return;
    }

    if (match(TokenKind::KW_VAR)) {
      consume(TokenKind::TK_IDENTIFIER);
      int local = locals_.add(parser_.prev().as_string());

      if (local == -1)
        error("local variable is already defined");

      consume(TokenKind::TK_EQ);

      // compile the initializer
      expression();
      emit_bytes(Code::STORE_LOCAL, local);
      return;
    }

    expression();
  }

  void expression(void) {
    call();
  }

  void call(void) {
    primary();
    if (match(TokenKind::TK_DOT)) {
      consume(TokenKind::TK_IDENTIFIER);
      int symbol = intern_symbol();

      // compile the method call
      emit_bytes(Code::CALL, symbol);
    }
  }

  void primary(void) {
    if (match(TokenKind::TK_LBRACE)) {
      auto* block = compile_block(parser_, this, TokenKind::TK_RBRACE);
      u8_t constant = block_->add_constant(block);

      emit_bytes(Code::CONSTANT, constant);
      return;
    }

    if (match(TokenKind::TK_IDENTIFIER)) {
      int local = locals_.get(parser_.prev().as_string());
      if (local == -1)
        error("unkonwn variable");

      emit_bytes(Code::LOAD_LOCAL, local);
      return;
    }

    if (match(TokenKind::TK_NUMERIC)) {
      numeric();
      return;
    }
  }
public:
  Compiler(Parser& parser, Compiler* parent = nullptr) noexcept
    : parser_(parser)
    , parent_(parent) {
    block_ = BlockObject::make_block();
  }

  ~Compiler(void) {
    // FIXME: fixed deallocate BlockObject by GC
  }

  BlockObject* compile_block(TokenKind end_kind) {
    for (;;) {
      statement();
      consume(TokenKind::TK_NL);

      if (match(end_kind))
        break;
      emit_byte(Code::POP);
    }
    emit_byte(Code::END);
    block_->set_num_locals(locals_.count());

    return parser_.had_error() ? nullptr : block_;
  }

  BlockObject* compile_block(Parser& p, Compiler* parent, TokenKind end_kind) {
    Compiler c(p, parent);
    return c.compile_block(end_kind);
  }
};

BlockObject* compile(VM& vm, const str_t& source_bytes) {
  Lexer lex(source_bytes);
  Parser p(vm, lex);

  p.advance();
  Compiler c(p, nullptr);

  return c.compile_block(TokenKind::TK_EOF);
}

}
