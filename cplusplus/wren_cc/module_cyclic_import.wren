
import "module_cyclic_import_a" for A

// expect: start a
// expect: a defined a value
// expect: start b
// expect: b defined b value
// expect: b imported a value
// expect: end b
// expect: a imported b value
// expect: end a
