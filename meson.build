project('cex', 'c', version : '0.1', default_options : ['warning_level=3'])

src_files = [
  'src/char_string.c',
  'src/argue.c',
]
incdir = include_directories('include')

libcex = library(
  'cex',
  src_files,
  include_directories : incdir,
  install : true
)

executable(
  'argue_app_example',
  'examples/argue_app.c',
  include_directories : incdir,
  link_with: libcex,
)

test(
  'test_char_string',
  executable(
    'test_char_string',
    'tests/test_char_string.c',
    include_directories : incdir,
    link_with : libcex,
  )
)
