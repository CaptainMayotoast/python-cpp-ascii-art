ranges_helpers_source = files('ranges_helpers.cpp')

ranges_helpers_include = include_directories('.')

lib_static_ranges_helpers = static_library(
    'ranges_helpers',
    ranges_helpers_source,
    cpp_args: cpp_flags,
    install: false,
    dependencies: [libpng_dep, libjpeg_turbo_dep, zlib_dep],
)

lib_static_ranges_helpers_dep = declare_dependency(
    link_with: lib_static_ranges_helpers,
    include_directories: ranges_helpers_include,
)
