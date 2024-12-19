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

// TODO: Rework image_to_ascii so it doesn't apply the crop / edge map
// that way a reference string can be acquired to put into this test case

// TEST_CASE("pixel_to_ascii", "[cudascii_pyfunctions]")
// {
//     cimg_library::CImg<unsigned char> src("assets/gradient.png");
//     REQUIRE(!src.is_empty());

//     unsigned char *ref = "o"

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
