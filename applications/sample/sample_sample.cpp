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

    std::optional<std::uint32_t> Sample::getPixel(SDL_Surface* surface, int row, int col)
    {
        // std::cout << "surface->w: " << surface->w << std::endl;
        if (row >= 0 && col >= 0 && col < surface->w && row < surface->h) {
            // int bpp = surface->format->BytesPerPixel;
            const auto pixels = reinterpret_cast<std::uint32_t*>(surface->pixels);
            auto pixel = pixels[row * surface->w + col]; // * bpp

            // assert(bpp == 1);

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

    std::string filepath = "test.bmp";

    SDL_SaveBMP(textSurface, filepath.c_str());

    SDL_Surface* freshSurface = SDL_ConvertSurfaceFormat(textSurface, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA8888, 0);

    for(int row  = 0; row < freshSurface->h; row++){
        for(int col = 0; col <freshSurface->w; col++){
            const auto value = std::to_string(getPixel(freshSurface, row, col).value_or(0));
            // if(value != "0")
            //     std::cout << std::format("val: {} row: {} col: {}", value, row, col) << std::endl;
            if (value != "0")
                std::cout << "1";
            else
                std::cout << "0";
        }
        std::cout << std::endl;
    }

    SDL_FreeSurface(textSurface);

    TTF_CloseFont(font);
}

}  // namespace sample