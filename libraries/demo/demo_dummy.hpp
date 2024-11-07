#include <pybind_pybind.hpp>

namespace demo {
int
add(const int i, const int j);

PYBIND11_MODULE(demo, m)
{
    m.doc() = "pybind11 demo plugin";  // optional module docstring

    m.def("add", &add, "A function that adds two numbers");
}

inline void
testSanitizer() noexcept
{
    char* s = reinterpret_cast<char*>(std::malloc(100));
    std::strcpy(s, "Hello world!");
    printf("string is: %s\n", s);
    // no free
}

}  // namespace demo
