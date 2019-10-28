
class Nil {}
class Bool {}
class Numeric {}
class Function {}
class Fiber {}

class Sequence {
  all(f) {
    var result = true
    for (element in this) {
      result = f.call(element)
      if (!result) return result
    }
    return result
  }

  any(f) {
    var result = false
    for (element in this) {
      result = f.call(element)
      if (result) return result
    }
    return result
  }

  contains(element) {
    for (item in this) {
      if (element == item) return true
    }
    return false
  }

  count {
    var result = 0
    for (element in this) {
      result = result + 1
    }
    return result
  }

  count(f) {
    var result = 0
    for (element in this) {
      if (f.call(element)) result = result + 1
    }
    return result
  }

  each(f) {
    for (element in this) {
      f.call(element)
    }
  }

  isEmpty { iterate(nil) ? false : true }

  map(transformation) { MapSequence.new(this, transformation) }

  where(predicate) { WhereSequence.new(this, predicate)}

  reduce(acc, f) {
    for (element in this) {
      acc = f.call(acc, element)
    }
    return acc
  }

  reduce(f) {
    var iter = iterate(nil)
    if (!iter) Fiber.abort("cannot reduce an empty sequence")

    // seed with the first element
    var result = iterValue(iter)
    while (iter = iterate(iter)) {
      result = f.call(result, iterValue(iter))
    }
    return result
  }

  join() { join("") }

  join(sep) {
    var first = true
    var result = ""

    for (element in this) {
      if (!first) result = result + sep
      first = false
      result = result + element.toString
    }
    return result
  }

  toList {
    var result = List.new()
    for (element in this) {
      result.add(element)
    }
    return result
  }
}

class MapSequence is Sequence {
  construct new(sequence, fn) {
    _sequence = sequence
    _fn = fn
  }

  iterate(iterator) { _sequence.iterate(iterator) }
  iterValue(iterator) { _fn.call(_sequence.iterValue(iterator)) }
}

class WhereSequence is Sequence {
  construct new(sequence, fn) {
    _sequence = sequence
    _fn = fn
  }

  iterate(iterator) {
    while (iterator = _sequence.iterate(iterator)) {
      if (_fn.call(_sequence.iterValue(iterator))) break
    }
    return iterator
  }

  iterValue(iterator) { _sequence.iterValue(iterator) }
}

class String is Sequence {
  bytes { StringByteSequence.new(this) }

  *(count) {
    if (!(count is Numeric) || !count.isInteger || count < 0) {
      Fiber.abort("count must be a non negative integer")
    }

    var result = ""
    for (i in 0...count) {
      result = result + this
    }
    return result
  }
}

class StringByteSequence is Sequence {
  construct new(string) {
    _string = string
  }

  [index] { _string.byteAt(index) }
  iterate(iterator) { _string.iterByte(iterator) }
  iterValue(iterator) { _string.byteAt(iterator) }
  count { _string.byteCount }
}

class List is Sequence {
  addAll(other) {
    for (element in other) {
      add(element)
    }
    return other
  }

  toString { "[" + join(", ") + "]" }

  +(other) {
    var result = this[0..-1]
    for (element in other) {
      result.add(element)
    }
    return result
  }

  *(count) {
    if (!(count is Numeric) || !count.isInteger || count < 0) {
      Fiber.abort("count must be a non negative integer")
    }

    var result = []
    for (i in 0...count) {
      result.addAll(this)
    }
    return result
  }
}

class Range is Sequence {}

class MapKeySequence is Sequence {
  construct new(map) {
    _map = map
  }

  iterate(n) { _map.iter(n) }
  iterValue(iterator) { _map.keyIterValue(iterator) }
}

class MapValSequence is Sequence {
  construct new(map) {
    _map = map
  }

  iterate(n) { _map.iter(n) }
  iterValue(iterator) { _map.valIterValue(iterator) }
}

class Map {
  keys { MapKeySequence.new(this) }
  values { MapValSequence.new(this) }

  toString {
    var first = true
    var result = "{"

    for (key in keys) {
      if (!first) result = result + ", "
      first = false
      result = result + key.toString + ": " + this[key].toString
    }

    return result + "}"
  }
}

class Sys {}
