#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark_all.hpp>

#include <nanobench.h>

#include <cstdint>
#include <execution>
#include <iostream>
#include <ranges>
#include <string_view>

#include <cudascii_utils.hpp>
#include <ranges_helpers.hpp>

namespace {

/// @brief represents Space
const constexpr uint8_t ASCIIStartIdx{32};

/// @brief represents ~
const constexpr uint8_t ASCIIEndIdx{126};

/// @brief the number of indices to cover (inclusive of last index)
const constexpr uint8_t ReadableASCIISize{ASCIIEndIdx - ASCIIStartIdx + 1};

/// @brief the max size in pixels that SDL will generate from a given ASCII character
const constexpr std::size_t MaxSizePixels{160};

/// @brief the min size in pixels that SDL will generate from a given ASCII character
const constexpr std::size_t MinSizePixels{128};

// 'm' in pixels
// 14 point font, 10px wide, 16 tall (14px + 1px padding + 1px padding)
const std::vector<std::uint8_t> Actual_M{
        0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,  0,   0,   0,   0,   0,   0,   0,   0,
        0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,  0,   0,   0,   0,   0,   0,   0,   0,
        1, 243, 203, 129, 241, 81,  163, 234, 63,  0,   0, 24, 244, 145, 101, 239, 116, 132, 163, 0,
        0, 8,   255, 32,  44,  250, 5,   80,  184, 0,   0, 8,  254, 3,   44,  225, 0,   80,  184, 0,
        0, 8,   252, 0,   44,  220, 0,   80,  184, 0,   0, 24, 252, 11,  44,  223, 8,   80,  190, 6,
        1, 243, 255, 178, 34,  253, 150, 68,  255, 126, 0, 0,  0,   0,   0,   0,   0,   0,   0,   0,
        0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,  0,   0,   0,   0,   0,   0,   0,   0,
        0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,  0,   0,   0,   0,   0,   0,   0,   0};

const std::vector<std::uint8_t> Actual_Space{
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

const std::vector<std::uint8_t> Actual_Tilda{
        0, 0, 0, 0,  0, 0,  0,   0,   0,  0, 0,   0,  0, 0,   0,  0,  0,   0,   0,   0,  0, 0,
        0, 0, 0, 0,  0, 0,  0,   0,   0,  0, 0,   0,  0, 0,   0,  0,  0,   0,   0,   0,  0, 0,
        0, 0, 0, 0,  0, 73, 224, 205, 86, 5, 102, 41, 0, 164, 68, 88, 214, 253, 197, 13, 0, 0,
        0, 0, 1, 25, 1, 0,  0,   0,   0,  0, 0,   0,  0, 0,   0,  0,  0,   0,   0,   0,  0, 0,
        0, 0, 0, 0,  0, 0,  0,   0,   0,  0, 0,   0,  0, 0,   0,  0,  0,   0,   0,   0,  0, 0,
        0, 0, 0, 0,  0, 0,  0,   0,   0,  0, 0,   0,  0, 0,   0,  0,  0,   0};

/// @brief generate a consteval array of char for readable ASCII characters
/// @note https://en.cppreference.com/w/cpp/language/ascii
/// @return the readable ASCII characters as a std::array
[[nodiscard]] consteval std::array<char, ReadableASCIISize>
generateASCIIChars() noexcept
{
    std::array<char, ReadableASCIISize> asciiChars;

    for (uint8_t i{0U}; i < ReadableASCIISize; ++i) {
        asciiChars[i] = static_cast<char>(i + ASCIIStartIdx);
    }

    return asciiChars;
}

}  // namespace

TEST_CASE("ascii_character_generation", "[cudascii_utils]")
{
    const constexpr auto ascii_characters{generateASCIIChars()};

    // confirm that the beginning and end are ' ' and '~' respectively
    CHECK(' ' == ascii_characters.front());
    REQUIRE('~' == ascii_characters.back());

    std::string_view sv_ascii_characters{ascii_characters};

    CHECK(' ' == sv_ascii_characters.front());
    REQUIRE('~' == sv_ascii_characters.back());
}

TEST_CASE("ascii_char_to_patch_m", "[cudascii_utils]")
{
    std::string_view m{"m"};

    const auto calculated_m = cudascii::utils::ascii_char_to_patch(m, 14);

    CHECK(calculated_m.size() == (16 * 10));
    REQUIRE(Actual_M == calculated_m);

    ankerl::nanobench::Bench().run("ascii_char_to_patch", [&]() {
        ankerl::nanobench::doNotOptimizeAway(cudascii::utils::ascii_char_to_patch(m, 14));
    });
}

TEST_CASE("ascii_chars_to_patchs", "[cudascii_utils]")
{
    std::string_view characters{"Nm45"};

    const auto calculated_patches = cudascii::utils::ascii_chars_to_patchs(characters, 14);

    CHECK(calculated_patches.size() == characters.size());
    CHECK(calculated_patches.at(characters.find('m')).size() == MaxSizePixels);
    REQUIRE(calculated_patches.at(characters.find('m')) == Actual_M);

    ankerl::nanobench::Bench().run("ascii_chars_to_patchs", [&]() {
        ankerl::nanobench::doNotOptimizeAway(cudascii::utils::ascii_chars_to_patchs(characters, 14));
    });
}

TEST_CASE("ascii_chars_to_patchs_front_back", "[cudascii_utils]")
{
    const constexpr auto ascii_characters = generateASCIIChars();
    auto sv_ascii_characters = std::string_view{ascii_characters};

    const auto calculated_patches = cudascii::utils::ascii_chars_to_patchs(sv_ascii_characters, 14);

    // Space
    CHECK(calculated_patches.size() == sv_ascii_characters.size());
    CHECK(calculated_patches.at(sv_ascii_characters.find(' ')).size() == MinSizePixels);
    REQUIRE(calculated_patches.at(sv_ascii_characters.find(' ')) == Actual_Space);

    // ~
    CHECK(calculated_patches.size() == sv_ascii_characters.size());
    CHECK(calculated_patches.at(sv_ascii_characters.find('~')).size() == MinSizePixels);
    REQUIRE(calculated_patches.at(sv_ascii_characters.find('~')) == Actual_Tilda);

    ankerl::nanobench::Bench().run("ascii_chars_to_patchs", [&]() {
        ankerl::nanobench::doNotOptimizeAway(
                cudascii::utils::ascii_chars_to_patchs(sv_ascii_characters, 14));
    });
}

TEST_CASE("ascii_chars_to_patchs_sizes", "[cudascii_utils]")
{
    const constexpr auto ascii_characters = generateASCIIChars();
    auto sv_ascii_characters = std::string_view{ascii_characters};

    const auto calculated_patches = cudascii::utils::ascii_chars_to_patchs(sv_ascii_characters, 14);

    const auto sizes = ranges_helpers::to_vector(
            calculated_patches | std::views::transform([](const auto vec) { return vec.size(); }));

    const auto [min, max] = std::minmax_element(std::execution::par_unseq, sizes.begin(), sizes.end());

    // 128
    CHECK(*min == MinSizePixels);

    // 160
    REQUIRE(*max == MaxSizePixels);
}
