#include <cudascii_pyfunctions.hpp>
#include <iostream>
#include <cuda_runtime.h>
#include <cimg_cimg.hpp>

namespace cudascii::pyfunctions {

void
pixel_to_ascii(
        unsigned char* out,
        unsigned char* r,
        unsigned char* g,
        unsigned char* b,
        int width,
        int height);
bool
test_cuda();

std::tuple<int, int, int>
get_image_size(const std::string& filename)
{
    // Load Image using CImg
    cimg_library::CImg<unsigned char> src(filename.c_str());

    // Get the image dimensions
    const int width = src.width();
    const int height = src.height();
    const int channels = src.spectrum();

    return {width, height, channels};
}

}  // namespace cudascii::pyfunctions
