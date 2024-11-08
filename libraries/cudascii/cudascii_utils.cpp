#include <cudascii_utils.hpp>

#include "spdlog/spdlog.h"

namespace cudascii::utils {

std::optional<std::uint8_t>
get_pixel(SDL_Surface* surface, int row, int col)
{
    if (row >= 0 && col >= 0 && col < surface->w && row < surface->h) {
        const auto pixels = reinterpret_cast<std::uint32_t*>(surface->pixels);
        auto pixel = pixels[row * surface->w + col];

        std::uint8_t r{0u};
        std::uint8_t g{0u};
        std::uint8_t b{0u};
        std::uint8_t a{0u};

        SDL_GetRGBA(
                pixel,
                surface->format,
                std::addressof(r),
                std::addressof(g),
                std::addressof(b),
                std::addressof(a));

        return r;
    }
    return {};
}

std::vector<std::uint8_t>
ascii_char_to_patch(std::string_view character, int patch_height)
{
    std::vector<std::uint8_t> patch;
    patch.reserve(patch_height * patch_height);

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        spdlog::error("TTF could not initialize! TTF_Error: {}", TTF_GetError());
    }

    TTF_Font* font =
            TTF_OpenFont("/build/applications/sample/assets/CourierPrime-Regular.ttf", patch_height);

    if (font == nullptr) {
        spdlog::error("{}", SDL_GetError());
    }

    SDL_Color foregroundColor = {255, 255, 255, 0};
    SDL_Color backgroundColor = {0, 0, 0, 0};

    SDL_Surface* textSurface = SDL_ConvertSurfaceFormat(
            TTF_RenderText_Shaded(font, character.data(), foregroundColor, backgroundColor),
            SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA8888,
            0);

    for (int row = 0; row < textSurface->h; row++) {
        for (int col = 0; col < textSurface->w; col++) {
            patch.push_back(get_pixel(textSurface, row, col).value_or(0));
        }
    }

    SDL_FreeSurface(textSurface);
    TTF_CloseFont(font);

    return patch;
}

std::vector<std::vector<std::uint8_t>>
ascii_chars_to_patchs(std::string_view chars, int patch_height)
{
    std::vector<std::vector<std::uint8_t>> pairs(chars.size());
    std::ranges::transform(chars, std::back_inserter(pairs), [patch_height](char c) {
        return ascii_char_to_patch(std::to_string(c), patch_height);
    });
    return pairs;
}

}  // namespace cudascii::utils
