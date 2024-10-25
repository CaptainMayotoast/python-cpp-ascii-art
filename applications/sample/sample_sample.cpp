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

    // const auto pixels = reinterpret_cast<std::uint32_t*>(textSurface->pixels);

    // std::uint8_t r{0u};
    // std::uint8_t g{0u};
    // std::uint8_t b{0u};
    // std::uint8_t a{0u};

    // std::vector<std::uint8_t> redPixels;
    // redPixels.reserve(textSurface->h * textSurface->w);

    // std::cout << textSurface->h << std::endl;
    // std::cout << textSurface->w << std::endl;

    // for(int i = 0; i < textSurface->h * textSurface->w; i++)
    // {
    //     SDL_GetRGBA(pixels[i], textSurface->format, std::addressof(r), std::addressof(g), std::addressof(b), std::addressof(a));
    //     redPixels.push_back(r);
    //     if(r != 0)
    //     {
    //         std::cout << std::to_string(r) << std::endl;
    //     }
    // }

    for(int row  = 0; row < textSurface->h; row++){
        for(int col = 0; col <textSurface->w; col++){
            const auto value = std::to_string(getPixel(textSurface, row, col).value_or(0));
            if(value != "0")
                std::cout << std::to_string(getPixel(textSurface, row, col).value_or(0)) << std::endl;
            // std::cout << (redPixels[row*textSurface->w+c] & 1U);
        }
        // std::cout << std::endl;
    }

    // for(int i = 0; i < textSurface->h * textSurface->w; i++)
    // {
    //     // SDL_GetRGBA(pixels[i], textSurface->format, std::addressof(r), std::addressof(g), std::addressof(b), std::addressof(a));
    //     const auto r = static_cast<std::uint32_t>(pixels[i]);
    //     redPixels.push_back(r);
    //     if(r != 0)
    //     {
    //         std::cout << std::format("{:#x}", r) << std::endl;
    //     }
    // }    

    SDL_FreeSurface(textSurface);

    TTF_CloseFont(font);

    // cimg_library::CImg<unsigned char> image(8 * 13, 14, 1, 3, 0); // 1x1 pixels, 3 color channels (RGB)

    // // Define the text to draw
    // const char* text = "alexander mig";
    
    // // Define the color (white)
    // unsigned char color[] = { 255, 255, 255 }; // RGB for white

    // // Draw the text on the image at position
    // image.draw_text(0, 0, text, color, 14); 

    // // Save the image to a file
    // image.save("output.png");  
}

// Sample::Sample()
// {
//     // https://stackoverflow.com/questions/3291923/how-to-get-rgb-value-by-cimg
//     cimg_library::CImg<unsigned char> src("/build/applications/sample/assets/kitten-000017380158_Smaller.jpg");
//     int width = src.width();
//     int height = src.height();
//     std::cout << width << "x" << height << std::endl;
//     for (int r = 0; r < height; r++)
//         for (int c = 0; c < width; c++)
//             std::cout << "(" << r << "," << c << ") ="
//                  << " R" << static_cast<int>(src(c, r, 0, 0)) << " G" << static_cast<int>(src(c, r, 0, 1)) << " B"
//                  << static_cast<int>(src(c, r, 0, 2)) << std::endl;
// }

}  // namespace sample