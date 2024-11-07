demo_source = files('demo_dummy.cpp')

lib_demo_dep_list = [dep_list, lib_static_pybind_dep]

py_installation.extension_module(
    'demo',
    sources: demo_source,
    dependencies: lib_demo_dep_list,
    cpp_args: cpp_flags,
    link_language: 'cpp',
    override_options: [
        'cpp_rtti=true',
        'cython_version=3',
        #'werror=true',
        'warning_level=3',
    ],
)
