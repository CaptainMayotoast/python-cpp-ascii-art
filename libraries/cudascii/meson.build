cudascii_source_pyfunctions = files('cudascii_pyfunctions.cpp')

cudascii_source_utils = files('cudascii_utils.cpp')

cudascii_include = include_directories('.')

lib_static_cudascii_utils = static_library(
    'lib_static_cudascii_utils',
    cudascii_source_utils,
    cpp_args: cpp_flags,
    dependencies: [dep_list, lib_static_cimg_dep],
)

lib_cudascii_utils_static_dep = declare_dependency(
    link_with: lib_static_cudascii_utils,
    include_directories: cudascii_include,
)

#######################################################################

cuda = import('unstable-cuda')

lib_static_cuda_cudascii = static_library(
    'cudascii_cuda',
    files('cudascii_pyfunctions.cu'),
    # https://docs.nvidia.com/cuda/cuda-compiler-driver-nvcc/index.html
    cuda_args: [cuda.nvcc_arch_flags('12.6', 'Common')]
    + [
        '--expt-relaxed-constexpr',
        '--std=c++20',
        '--diag-suppress=177', # supress unused function warning
        '-cudart=static',
        '-cudadevrt=static',
    ],
    link_args: ['-static-libstdc++'],
    install: false,
    dependencies: [
        catch2_with_main_dep,
        cuda_dep,
        lib_static_cimg_dep,
        libcudart_dep,
        sdl2_dep,
        sdl2_ttf_dep,
        zlib_dep,
    ],
)

lib_static_cuda_cudascii_dep = declare_dependency(link_with: lib_static_cuda_cudascii, include_directories: cudascii_include)

#######################################################################

lib_cudascii_dep_list = [
    cli11_core_dep,
    cuda_dep,
    lib_static_cimg_dep,
    lib_static_cuda_cudascii_dep,
    lib_cudascii_utils_static_dep,
    libcudart_dep,
    py_dep,
    pybind11_dep,
    sdl2_dep,
    sdl2_ttf_dep,
    spdlog_dep,
    zlib_dep,
]

# https://mesonbuild.com/Python-module.html#extension_module
py_installation.extension_module(
    'cudascii',
    sources: [cudascii_source_pyfunctions, cudascii_source_utils],
    dependencies: lib_cudascii_dep_list,
    link_language: 'cpp',
    link_args: ['-static-libstdc++'],
    override_options: ['cpp_rtti=true', 'cython_version=3'],
)

#######################################################################

# use catch2 for the test framework
# use nanobench for benchmarking
test_cudascii_utils = executable(
    'cudascii_utilities_tests',
    [cudascii_source_utils, 'cudascii_utils.t.cpp'],
    dependencies: [
        sdl2_dep,
        sdl2_ttf_dep,
        lib_static_cimg_dep,
        spdlog_dep,
        catch2_with_main_dep,
        nanobench_dep,
        lib_static_ranges_helpers_dep,
    ],
)
test('cudascii_utilities_tests', test_cudascii_utils)
