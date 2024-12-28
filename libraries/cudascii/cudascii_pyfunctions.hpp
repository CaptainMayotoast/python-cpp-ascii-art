#include <pybind11/pybind11.h>

#include <cstdlib>

namespace cudascii::pyfunctions {

////////////////////////////////////////////////////////////////////
// CUDA functions

/**
 * @brief CUDA kernel function mapping an RGB pixel to a gray level char
 *
 * @param out out parameter for ASCII characters
 * @param r the array of unsigned char representing the RED channel
 * @param g the array of unsigned char representing the GREEN channel
 * @param b the array of unsigned char representing the BLUE channel
 * @param width width of the image
 * @param height height of the image
 */
void
pixel_to_ascii_kernel(
        unsigned char* out,
        unsigned char* r,
        unsigned char* g,
        unsigned char* b,
        int width,
        int height);

// void
// generate_ascii_from_patch_comparison_kernel(
//         unsigned char* r,
//         unsigned char* g,
//         unsigned char* b,
//         int width,
//         int height,
//         unsigned char* out);

/**
 *  @brief Maps an RGB pixel to a gray level char
 *
 * @param out Out parameter for ASCII characters
 * @param r The array of unsigned char representing the RED channel
 * @param g The array of unsigned char representing the GREEN channel
 * @param b The array of unsigned char representing the BLUE channel
 * @param i The pixel index
 */
void
pixel_to_ascii(unsigned char* out, unsigned char* r, unsigned char* g, unsigned char* b, int i);

////////////////////////////////////////////////////////////////////
// Python functions within the module

/**
 * @brief Convert an image to ASCII (CPU)
 *
 * @param filename the image to convert
 * @param patch_width the width in pixels of a character (defaulted to 8, which appears to be the width
 * of an Ubuntu monospace font)
 * @param patch_height the height in pixels of a character (defaulted to 14, which appears to be the
 * height of an Ubuntu monospace font)
 * @return std::string the ASCII representation of an image
 */
[[nodiscard]] std::string
image_to_ascii_cpu(const std::string& filename, int patch_width = 8u, int patch_height = 14u);

/**
 * @brief Convert an image to ASCII (GPU)
 *
 * @param filename the image to convert
 * @param patch_width the width in pixels of a character (defaulted to 8, which appears to be the width
 * of an Ubuntu monospace font)
 * @param patch_height the height in pixels of a character (defaulted to 14, which appears to be the
 * height of an Ubuntu monospace font)
 * @return std::string the ASCII representation of an image
 */
[[nodiscard]] std::string
image_to_ascii_gpu(const std::string& filename, int patch_width = 8u, int patch_height = 14u);

/**
 * @brief Convert an image to ASCII
 *
 * @param filename the image to convert
 * @param patch_width the width in pixels of a character (defaulted to 8, which appears to be the width
 * of an Ubuntu monospace font)
 * @param patch_height the height in pixels of a character (defaulted to 14, which appears to be the
 * height of an Ubuntu monospace font)
 * @return std::string the ASCII representation of an image
 */
[[nodiscard]] std::string
image_to_ascii(
        const std::string& filename,
        int patch_width = 8u,
        int patch_height = 14u,
        bool useCpu = false);

/**
 * @brief Gets the width, height and channels of the image
 *
 * @param filename The file to process
 */
[[nodiscard]] std::tuple<int, int, int>
get_image_size(const std::string& filename);

PYBIND11_MODULE(cudascii, m)
{
    m.doc() = "pybind11 cimg example plugin";  // optional module docstring

    m.def("get_image_size",
          &get_image_size,
          "A function that reads an image and returns its dimensions");
    m.def("image_to_ascii", &image_to_ascii, "A function to convert an image to ascii text");
}

}  // namespace cudascii::pyfunctions
