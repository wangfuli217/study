
IO.print([].toString) // expect: []

// does not quote strings
IO.print([1, "2", true].toString) // expect: [1, 2, true]

// nested lists
IO.print([1, [2, [3], 4], 5]) // expect: [1, [2, [3], 4], 5]

// calls toString on elements
class Foo {
  construct new() {}
  toString { "Foo.toString" }
}
IO.print([1, Foo.new(), 2]) // expect: [1, Foo.toString, 2]
