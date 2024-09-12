#include <sample_sample.hpp>

#include <iostream>

#include <cimg_cimg.hpp>

namespace sample {

Sample::Sample(){
    cimg_library::CImg<unsigned char> image(8, 14, 1, 3, 0); // 1x1 pixels, 3 color channels (RGB)

    // Define the text to draw
    const char* text = "F";
    
    // Define the color (white)
    unsigned char color[] = { 255, 255, 255 }; // RGB for white

    // Draw the text on the image at position
    image.draw_text(0, 0, text, color, 14); 

    // Save the image to a file
    image.save("output.png");  
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