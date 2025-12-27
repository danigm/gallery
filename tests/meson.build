test_src = [
  '../src/gly.c',
  '../src/gly.h',
  '../src/parser.c',
  '../src/parser.h',
  'test.c',
]

exe = executable('gly_test', test_src, dependencies: deps)
test('gallery tests', exe, workdir : meson.source_root())
