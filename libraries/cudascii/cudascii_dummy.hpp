#include <pybind11/pybind11.h>

#include <cstdlib>

namespace cudascii {

    ////////////////////////////////////////////////////////////////////
    // CUDA functions

    // Cuda kernel function mapping an RGB pixel to a gray level char
    void pixel_to_ascii(unsigned char *out, unsigned char *r, unsigned char *g, unsigned char *b, int width, int height);
    
    ////////////////////////////////////////////////////////////////////
    // Python functions within the module

    bool test_cuda();

    /**
     * @brief Convert an image to ASCII
     * 
     * @param filename the image to convert
     * @param patch_width the width in pixels of a character (defaulted to 8, which appears to be the width of an Ubuntu monospace font)
     * @param patch_height the height in pixels of a character (defaulted to 14, which appears to be the height of an Ubuntu monospace font)
     * @return std::string the ASCII representation of an image
     */
    std::string image_to_ascii(const std::string &filename, int patch_width = 8u, int patch_height = 14u);
    std::tuple<int, int, int> get_image_size(const std::string &filename);

    

    PYBIND11_MODULE(cudascii, m) {
        m.doc() = "pybind11 cimg example plugin"; // optional module docstring

        m.def("get_image_size", &get_image_size, "A function that reads an image and returns its dimensions");
        m.def("image_to_ascii", &image_to_ascii, "A function to convert an image to ascii text");
        m.def("test_cuda", &test_cuda, "A function to test cuda setting a vector");
    }

}
