#include <SDL.h>
#include <SDL_ttf.h>
#include <cimg_cimg.hpp>
#include <cstdint>
#include <filesystem>
#include <numeric>
#include <optional>
#include <ranges>
#include <span>
#include <string_view>
#include <vector>

namespace cudascii::utils {

/// @brief represents Space
const constexpr uint8_t ASCIIStartIdx{32U};

/// @brief represents ~
const constexpr uint8_t ASCIIEndIdx{126U};

/// @brief the number of indices to cover (inclusive of last index)
const constexpr uint8_t ReadableASCIISize{ASCIIEndIdx - ASCIIStartIdx + 1};

/// @brief generate a consteval array of char for readable ASCII characters
/// @note https://en.cppreference.com/w/cpp/language/ascii
/// @return the readable ASCII characters as a std::array
[[nodiscard]] consteval std::array<char, ReadableASCIISize>
generateASCIIChars() noexcept
{
    std::array<char, ReadableASCIISize> asciiChars;

    for (uint8_t i : std::views::iota(0U, ReadableASCIISize)) {
        asciiChars[i] = static_cast<char>(i + ASCIIStartIdx);
    }

    return asciiChars;
}

/// @brief gets the pixel from an SDL_Surface by row and col
/// @param surface the SDL_Surface to extract a pixel from
/// @param row the row (y value)
/// @param col the column (x value)
/// @return the pixel's single channel (red), or an empty optional if the pixel retrieval was out of
/// bounds
[[nodiscard]] std::optional<std::uint8_t>
get_pixel(const SDL_Surface* surface, int row, int col);

[[nodiscard]] std::vector<std::uint8_t>
ascii_char_to_patch(
        std::string_view character,
        int patch_height = 14,
        const std::filesystem::path& ttf_path = {
                "/build/applications/sample/assets/CourierPrime-Regular.ttf"});

[[nodiscard]] std::vector<std::vector<std::uint8_t>>
ascii_chars_to_patchs(
        std::string_view chars,
        int patch_height = 14,
        const std::filesystem::path& ttf_path = {
                "/build/applications/sample/assets/CourierPrime-Regular.ttf"});

[[nodiscard]] cimg_library::CImg<unsigned char>
crop_to_grid(cimg_library::CImg<unsigned char> src, int cell_width, int cell_height);

[[nodiscard]] cimg_library::CImg<unsigned char>
edge_map(cimg_library::CImg<unsigned char> src);

}  // namespace cudascii::utils
