
class Foo {
  static test {
    IO.print(this == Foo) // expect: true
    IO.print(this.bar) // expect: bar
  }

  static bar { return "bar" }
}

Foo.test
