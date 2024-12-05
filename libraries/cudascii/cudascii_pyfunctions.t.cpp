#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark_all.hpp>

#include <nanobench.h>

#include <cstdint>
#include <execution>
#include <iostream>
#include <ranges>
#include <string_view>

#include <cudascii_pyfunctions.hpp>
#include <ranges_helpers.hpp>

// TEST_CASE("pixel_to_ascii", "[cudascii_pyfunctions]")
// {
//     cimg_library::CImg<unsigned char> src("assets/gradient.png");
//     REQUIRE(!src.is_empty());

//     unsigned char *ref = ""

//     int width{src.width()};
//     int height{src.height()};

//     unsigned char *out, *r, *g, *b;
//     r = src.channel(0);
//     g = src.channel(1);
//     b = src.channel(2);

//     for (int i = 0; i < width * height; i++)
//     {
//         pixel_to_ascii(out, r, g, b, i);
//         CHECK(out[i] == ref[i]);
//     }

// }
