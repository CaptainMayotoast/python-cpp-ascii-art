cimg_source = files('cimg_cimg.cpp')

cimg_include = include_directories('.')

lib_static_cimg = static_library(
    'cimg',
    cimg_source,
    cpp_args: cpp_flags,
    install: false,
    dependencies: [libpng_dep, libjpeg_turbo_dep, zlib_dep],
)

lib_static_cimg_dep = declare_dependency(
    link_with: lib_static_cimg,
    include_directories: cimg_include,
)
