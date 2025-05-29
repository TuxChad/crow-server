project('crow-server', 'cpp', default_options: ['cpp_std=c++17'])

sources_file = ['src/main.cpp']
include_dirs = include_directories('include')

executable(
  'crow-server',
  sources_file,
  cpp_args: ['-DCROW_USE_BOOST'],
  include_directories: include_dirs,
  dependencies: [dependency('libpqxx'), dependency('threads')],
)
