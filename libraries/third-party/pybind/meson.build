pybind_source = files('pybind_pybind.cpp')

pybind_include = include_directories('.')

lib_pybind_dep_list = [pybind11_dep, py_dep]

lib_static_pybind = static_library(
    'pybind',
    pybind_source,
    cpp_args: cpp_flags,
    install: false,
    dependencies: lib_pybind_dep_list,
)

lib_static_pybind_dep = declare_dependency(
    link_with: lib_static_pybind,
    include_directories: pybind_include,
    dependencies: lib_pybind_dep_list,
)

# dep_list += lib_static_pybind_dep
