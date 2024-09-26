#include <sample_sample.hpp>

#include <iostream>

#include <cimg_cimg.hpp>

#include <SDL.h>
#include <SDL_ttf.h>

#include <stdio.h>
#include <cassert>

namespace sample {

Sample::Sample(){

    // // Initialize SDL
    // if (SDL_Init() < 0) {
    //     printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    // }

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

    SDL_Surface* textSurface
    = TTF_RenderText_Shaded
    (font, "m", foregroundColor, backgroundColor);

    assert(textSurface != nullptr);

    std::string filepath = "blah.bmp";

    SDL_SaveBMP(textSurface, filepath.c_str());

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