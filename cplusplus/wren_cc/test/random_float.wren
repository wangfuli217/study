
import "random" for Random

var random = Random.new(12345)

var below = 0
for (i in 1..1000) {
  var n = random.float()
  if (n < 0.5) below = below + 1
}

IO.print(below > 450) // expect: true
IO.print(below < 550) // expect: true
