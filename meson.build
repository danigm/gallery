project(
  'gallery', 'c',
  version: '1.0',
  license: 'GPL3+',
)

# Sources
headers = [
  'src/parser.h',
]

src = [
  'src/parser.c',
  'src/main.c',
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
