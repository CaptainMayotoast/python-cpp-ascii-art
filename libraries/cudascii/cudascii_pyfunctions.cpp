#include <cudascii_pyfunctions.hpp>

#include <iostream>

#include <cimg_cimg.hpp>
#include <cuda_runtime.h>
#include <cudascii_utils.hpp>

namespace {

// Algorithm Parameterization
__device__ const constexpr auto gray_levels_fine = cudascii::utils::generate_ascii_chars();
__device__ const constexpr int gray_levels_fine_count = gray_levels_fine.size();
auto gray_levels_fine_sv = std::string_view{gray_levels_fine.data()};

const constexpr float RED_WEIGHT = 0.2126;
const constexpr float GREEN_WEIGHT = 0.7152;
const constexpr float BLUE_WEIGHT = 0.0722;
const constexpr float CONVERSION_THRESHOLD = 0.0031308;
const constexpr float BELOW_THRESHOLD_SCALAR = 12.92;
const constexpr float ABOVE_THRESHOLD_SCALAR = 1.055;
const constexpr float ABOVE_THRESHOLD_EXPONENT = 1 / 2.4;
const constexpr float ABOVE_THRESHOLD_OFFSET = -0.055;

}  // namespace

namespace cudascii::pyfunctions {

// void
// pixel_to_ascii(
//         unsigned char* out,
//         unsigned char* r,
//         unsigned char* g,
//         unsigned char* b,
//         int width,
//         int height);

// std::string
// image_to_ascii_cpu([[maybe_unused]] const std::string& filename, [[maybe_unused]] int patch_width,
// [[maybe_unused]] int patch_height)
// {
//     [[maybe_unused]] const auto char_patches =
//             cudascii::utils::ascii_chars_to_patchs(gray_levels_fine_sv, 14);

//     return "";
// }

std::tuple<int, int, int>
get_image_size(const std::string& filename)
{
    // Load Image using CImg
    const cimg_library::CImg<unsigned char> src(filename.c_str());

    return {src.width(), src.height(), src.spectrum()};
}

}  // namespace cudascii::pyfunctions
