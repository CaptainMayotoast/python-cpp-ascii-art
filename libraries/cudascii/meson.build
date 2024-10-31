cudascii_source = files('cudascii_dummy.cpp', 'cudascii_utils.cpp')

cudascii_include = include_directories('.')

cuda = import('unstable-cuda')

lib_static_cuda_cudascii = static_library('cudascii_cuda',
                            files('cudascii_dummy.cu'),
                            cuda_args : [cuda.nvcc_arch_flags('12.4', 'Common')] + ['--expt-relaxed-constexpr', 
                                                                                                                        '--std=c++20', 
                                                                                                                        '--diag-suppress=177' # supress unused function warning
                                                                                                                        ],
                            link_args: ['-static-libstdc++'],
                            install: false,
                            dependencies: [zlib_dep, sdl2_dep, sdl2_ttf_dep, cuda_dep, libcudart_dep, lib_static_cimg_dep, catch2_with_main_dep])
 
lib_static_cuda_cudascii_dep = declare_dependency(link_with: lib_static_cuda_cudascii,
                                                  include_directories: cudascii_include)

lib_cudascii_dep_list = [
                    zlib_dep, sdl2_dep, sdl2_ttf_dep,
                    cli11_core_dep,
                    cuda_dep,
                    lib_static_cimg_dep,
                    spdlog_dep,
                    pybind11_dep,
                    py_dep,
                    lib_static_cuda_cudascii_dep]

# https://mesonbuild.com/Python-module.html#extension_module
py_installation.extension_module('cudascii',
    sources : cudascii_source, 
    dependencies : lib_cudascii_dep_list,
    link_language : 'cpp',
    link_args: ['-static-libstdc++'],
    override_options: [
        'cpp_rtti=true',
        'cython_version=3'        
    ]
)