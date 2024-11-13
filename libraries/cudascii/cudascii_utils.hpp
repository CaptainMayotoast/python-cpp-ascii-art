#include <SDL.h>
#include <SDL_ttf.h>

#include <cstdint>
#include <filesystem>
#include <optional>
#include <span>
#include <string_view>
#include <vector>

namespace cudascii::utils {

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

}  // namespace cudascii::utils
