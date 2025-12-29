project(
  'gallery', 'c',
  version: '1.0',
  license: 'GPL3+',
)

# Sources
headers = [
  'src/gly.h',
  'src/img.h',
  'src/parser.h',
  'src/writer.h',
]

src = [
  'src/gly.c',
  'src/img.c',
  'src/main.c',
  'src/parser.c',
  'src/writer.c',
]

# Dependencies
deps = [
  dependency('gio-2.0'),
  dependency('glib-2.0'),
  dependency('libxml2'),
  dependency('libexif'),
]

executable('gallery', src + headers, dependencies: deps)


subdir('tests')
