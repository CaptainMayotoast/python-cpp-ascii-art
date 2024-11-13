#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark_all.hpp>

#include <cstdint>
#include <iostream>

#include <cudascii_utils.hpp>

namespace {

// 'm' in pixels
// 14 point font, 10px wide, 16 tall (14px + 1px padding + 1px padding)
const std::vector<std::uint8_t> actual_m{
        0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,  0,   0,   0,   0,   0,   0,   0,   0,
        0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,  0,   0,   0,   0,   0,   0,   0,   0,
        1, 243, 203, 129, 241, 81,  163, 234, 63,  0,   0, 24, 244, 145, 101, 239, 116, 132, 163, 0,
        0, 8,   255, 32,  44,  250, 5,   80,  184, 0,   0, 8,  254, 3,   44,  225, 0,   80,  184, 0,
        0, 8,   252, 0,   44,  220, 0,   80,  184, 0,   0, 24, 252, 11,  44,  223, 8,   80,  190, 6,
        1, 243, 255, 178, 34,  253, 150, 68,  255, 126, 0, 0,  0,   0,   0,   0,   0,   0,   0,   0,
        0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,  0,   0,   0,   0,   0,   0,   0,   0,
        0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,  0,   0,   0,   0,   0,   0,   0,   0};

}  // namespace

TEST_CASE("ascii_char_to_patch_m", "[cudascii_utils]")
{
    std::string_view m{"m"};

    const auto calculated_m = cudascii::utils::ascii_char_to_patch(m, 14);

    CHECK(calculated_m.size() == (16 * 10));
    REQUIRE(actual_m == calculated_m);

    BENCHMARK("ascii_char_to_patch 'm'")
    {
        return cudascii::utils::ascii_char_to_patch(m, 14);
    };
}

TEST_CASE("ascii_chars_to_patchs", "[cudascii_utils]")
{
    std::string_view characters{"Nm45"};

    const auto calculated_patches = cudascii::utils::ascii_chars_to_patchs(characters, 14);

    CHECK(calculated_patches.size() == characters.size());
    CHECK(calculated_patches.at(characters.find('m')).size() == (16 * 10));
    REQUIRE(calculated_patches.at(characters.find('m')) == actual_m);
}
