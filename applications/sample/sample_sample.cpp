#include <sample_sample.hpp>

#include <iostream>

#include <cimg_cimg.hpp>

#include <SDL.h>
#include <SDL_ttf.h>

#include <cassert>
#include <cstdint>
#include <format>
#include <stdio.h>
#include <vector>

namespace sample {

    std::optional<std::uint32_t> Sample::getPixel(SDL_Surface* surface, int x, int y)
    {
        if (x >= 0 && y >= 0 && x < surface->w && y < surface->h) {
            int bpp = surface->format->BytesPerPixel;
            const auto pixels = reinterpret_cast<std::uint8_t*>(surface->pixels);
            auto pixel = pixels[y * surface->pitch + x * bpp];

            assert(bpp == 1);

            std::uint8_t r{0u};
            std::uint8_t g{0u};
            std::uint8_t b{0u};
            std::uint8_t a{0u};

            SDL_GetRGBA(static_cast<std::uint32_t>(pixel), surface->format, std::addressof(r), std::addressof(g), std::addressof(b), std::addressof(a));

            return static_cast<std::uint32_t>(r);
        }
        return {};
    }

Sample::Sample(){
    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        printf("TTF could not initialize! TTF_Error: %s\n", TTF_GetError());
    }

    TTF_Font* font = TTF_OpenFont("/build/applications/sample/assets/CourierPrime-Regular.ttf", 14);

    if(font == nullptr){
        std::cout << SDL_GetError() << std::endl;
    }

    SDL_Color foregroundColor = { 255, 255, 255, 0 };
    SDL_Color backgroundColor = { 0, 0, 0, 0 };

    SDL_Surface* textSurface = TTF_RenderText_Shaded(font, "m", foregroundColor, backgroundColor);

    assert(textSurface != nullptr);

    std::string filepath = "blah.bmp";

    SDL_SaveBMP(textSurface, filepath.c_str());

    for(int row  = 0; row < textSurface->h; row++){
        for(int col = 0; col <textSurface->w; col++){
            const auto value = std::to_string(getPixel(textSurface, row, col).value_or(0));
            if(value != "0")
                std::cout << std::to_string(getPixel(textSurface, row, col).value_or(0)) << std::endl;
        }
    }

    SDL_FreeSurface(textSurface);

    TTF_CloseFont(font);
}

}  // namespace sample