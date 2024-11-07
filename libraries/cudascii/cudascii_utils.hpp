#include <SDL.h>
#include <SDL_ttf.h>

#include <cstdint>
#include <optional>
#include <span>
#include <string_view>
#include <vector>

namespace cudascii::utils {

[[nodiscard]] std::optional<std::uint8_t>
get_pixel(SDL_Surface* surface, int row, int col);

[[nodiscard]] std::vector<std::uint8_t>
ascii_char_to_patch(char* character, int patch_width, int patch_height = 14);

[[nodiscard]] std::vector<std::vector<std::uint8_t>>
ascii_chars_to_patchs(std::string_view chars, int patch_width, int patch_height = 14);

}  // namespace cudascii::utils
