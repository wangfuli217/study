
import "module_multi_vars_module" for Module1
import "module_multi_vars_module" for Module2
import "module_multi_vars_module" for Module3
import "module_multi_vars_module" for Module4
import "module_multi_vars_module" for Module5

IO.print(Module1) // expect: from module 1
IO.print(Module2) // expect: from module 2
IO.print(Module3) // expect: from module 3
IO.print(Module4) // expect: from module 4
IO.print(Module5) // expect: from module 5
