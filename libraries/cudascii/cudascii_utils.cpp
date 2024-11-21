#include <cudascii_utils.hpp>

#include <spdlog/spdlog.h>

#include <execution>
#include <ranges>

namespace cudascii::utils {

std::optional<std::uint8_t>
get_pixel(const SDL_Surface* surface, int row, int col)
{
    if (row >= 0 && col >= 0 && col < surface->w && row < surface->h) [[likely]] {
        const auto pixels = static_cast<std::uint32_t*>(surface->pixels);

        std::uint8_t r{0u};
        [[maybe_unused]] std::uint8_t g{0u};
        [[maybe_unused]] std::uint8_t b{0u};
        [[maybe_unused]] std::uint8_t a{0u};

        SDL_GetRGBA(
                pixels[row * surface->w + col],
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
ascii_char_to_patch(std::string_view character, int patch_height, const std::filesystem::path& ttf_path)
{
    // Initialize SDL_ttf
    if (TTF_Init() == -1) [[unlikely]] {
        spdlog::error("TTF could not initialize! TTF_Error: {}", TTF_GetError());
    }

    TTF_Font* font = TTF_OpenFont(ttf_path.c_str(), patch_height);

    if (font == nullptr) [[unlikely]] {
        spdlog::error("{}", SDL_GetError());
    }

    const SDL_Color foregroundColor = {255, 255, 255, 0};
    const SDL_Color backgroundColor = {0, 0, 0, 0};

    const char singleChar{character.front()};

    SDL_Surface* textSurface = SDL_ConvertSurfaceFormat(
            TTF_RenderText_Shaded(font, std::addressof(singleChar), foregroundColor, backgroundColor),
            SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA8888,
            0);

    std::vector<std::uint8_t> patch;
    patch.reserve(textSurface->h * textSurface->w);

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
ascii_chars_to_patchs(std::string_view chars, int patch_height, const std::filesystem::path& ttf_path)
{
    std::vector<std::vector<std::uint8_t>> chars_as_patches(chars.size());
    std::transform(
            std::execution::par_unseq,
            chars.cbegin(),
            chars.cend(),
            chars_as_patches.begin(),
            [patch_height, &ttf_path](const auto c) {
                return ascii_char_to_patch(std::string_view{&c, 1}, patch_height, ttf_path);
            });
    return chars_as_patches;
}

cimg_library::CImg<unsigned char>
crop_to_grid(cimg_library::CImg<unsigned char> src, int cell_width, int cell_height)
{
    int width{src.width()};
    int height{src.height()};

    // clip image to the cell size
    int crop_width = (width / cell_width) * cell_width;
    int crop_height = (height / cell_height) * cell_height;

    return src.resize(crop_width, crop_height);
}

cimg_library::CImg<unsigned char>
edge_map(cimg_library::CImg<unsigned char> src)
{
    // this is edge detection
    // https://en.wikipedia.org/wiki/Image_gradient
    // https://cimg.eu/reference/structcimg__library_1_1CImg.html#a6c7b2bc4442e062706fa1bbc04621d8f
    auto gradientList = src.get_gradient("xy", 0);

    // get_gradient returns two elements here, gradient in the north-south direction and in the east-west
    // direction abs is taken because we do not care about negative values, sign (+/-) indicates a side,
    // but we do not care about which side the gradient value is on) gray is no gradient, black would be
    // negative, white is positive
    return cimg_library::CImg<unsigned char>(gradientList.at(0).abs() + gradientList.at(1).abs());
}

}  // namespace cudascii::utils
