
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

  map(transformation) { new MapSequence(this, transformation) }

  where(predicate) { new WhereSequence(this, predicate)}

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

  join { join("") }

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
    var result = new List
    for (element in this) {
      result.add(element)
    }
    return result
  }
}

class MapSequence is Sequence {
  new(sequence, fn) {
    _sequence = sequence
    _fn = fn
  }

  iterate(iterator) { _sequence.iterate(iterator) }
  iterValue(iterator) { _fn.call(_sequence.iterValue(iterator)) }
}

class WhereSequence is Sequence {
  new(sequence, fn) {
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
  bytes { new StringByteSequence(this) }
}

class StringByteSequence is Sequence {
  new(string) {
    _string = string
  }

  [index] { _string.byteAt(index) }
  iterate(iterator) { _string.iterByte(iterator) }
  iterValue(iterator) { _string.byteAt(iterator) }
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
}

class Range is Sequence {}

class MapKeySequence is Sequence {
  new(map) {
    _map = map
  }

  iterate(n) { _map.iter(n) }
  iterValue(iterator) { _map.keyIterValue(iterator) }
}

class MapValSequence is Sequence {
  new(map) {
    _map = map
  }

  iterate(n) { _map.iter(n) }
  iterValue(iterator) { _map.valIterValue(iterator) }
}

class Map {
  keys { new MapKeySequence(this) }
  values { new MapValSequence(this) }

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
