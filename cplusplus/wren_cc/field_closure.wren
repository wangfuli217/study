
class Foo {
  new { _field = "Foo._field" }

  closeOverGet {
    return fn { return _field }
  }

  closeOverSet {
    return fn { _field = "new value" }
  }
}

var foo = new Foo
IO.write(foo.closeOverGet.call) // expect: Foo._field
foo.closeOverSet.call
IO.write(foo.closeOverGet.call) // expect: new value
