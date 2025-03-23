project('cex', 'c', version : '0.1', default_options : ['warning_level=3'])

src_files = [
  'src/char_string.c',
  'src/panic.c'
]
incdir = include_directories('include')

libcex = library(
  'cex',
  src_files,
  include_directories : incdir,
  install : true
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
