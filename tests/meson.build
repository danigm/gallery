test_src = [
  '../src/parser.h',
  '../src/parser.c',
  'test.c',
]

exe = executable('gly_test', test_src, dependencies: deps)
test('gallery tests', exe, workdir : meson.source_root())
