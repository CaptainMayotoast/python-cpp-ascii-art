#include <cuda_runtime.h>

#include <chrono>
#include <cmath>
#include <iostream>
#include <string>
#include <thread>
#include <cimg_cimg.hpp>

#include <cudascii_utils.hpp>

namespace {

// Algorithm Parameterization
__device__ const constexpr auto gray_levels_fine = cudascii::utils::generateASCIIChars();
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

__global__ void
patch_to_ascii(
        unsigned char* out,
        unsigned char* image,
        unsigned char* ref_patches,
        int patch_width,
        int patch_height,
        int num_patch_columns)
{
    // Calculate the global patch index
    int patch_x = blockIdx.x * blockDim.x + threadIdx.x;
    int patch_y = blockIdx.y * blockDim.y + threadIdx.y;

    // if (!(x < width && y < height))
    //     return;

    int patch_index = patch_y * num_patch_columns + patch_x;
    // Thread index
    // int i = threadIdx.x + blockIdx.x * blockDim.x;

    unsigned char matched_char{' '};
    unsigned int min_distance{std::numeric_limits<int>::max()};

    for (int reference_char_index = 0; reference_char_index < gray_levels_fine_count; reference_char_index++) {
        unsigned int distance{0};

        for (int row = 0; row < patch_height; row++) {
            for (int col = 0; col < patch_width; col++) {
                unsigned int reference_pixel_index =
                        (reference_char_index * patch_height * patch_width) + row * patch_width + col;
                unsigned int patch_pixel_index =
                        (patch_y * patch_height + row) * (num_patch_columns * patch_width)
                        + (patch_x * patch_width) + col;

                unsigned char patch_pixel = image[patch_pixel_index];
                unsigned char reference_pixel = ref_patches[reference_pixel_index];

                if (reference_pixel > patch_pixel)
                    distance += (reference_pixel - patch_pixel);
                else
                    distance += (patch_pixel - reference_pixel);
            }
        }

        if (distance < min_distance) {
            matched_char = gray_levels_fine[reference_char_index];
            min_distance = distance;
        }
    }

    out[patch_index] = matched_char;
}

__global__ void
pixel_to_ascii(
        unsigned char* out,
        unsigned char* r,
        unsigned char* g,
        unsigned char* b,
        int width,
        int height)
{
    // Calculate the global pixel index
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (!(x < width && y < height)) return;

    int i = y * width + x;
    // Thread index
    // int i = threadIdx.x + blockIdx.x * blockDim.x;

    float c_linear, c_srgb;
    int gray_index{0};

    // Standard linear combination
    c_linear = RED_WEIGHT * (r[i] / 255.) + GREEN_WEIGHT * (g[i] / 255.) + BLUE_WEIGHT * (b[i] / 255.);

    // If gray level is very dark, use linear scaling
    if (c_linear <= CONVERSION_THRESHOLD) c_srgb = BELOW_THRESHOLD_SCALAR * c_linear;

    // Non linear scaling to adjust for gamma exposure
    else
        c_srgb = ABOVE_THRESHOLD_SCALAR * powf(c_linear, ABOVE_THRESHOLD_EXPONENT)
                 + ABOVE_THRESHOLD_OFFSET;

    // Scale c_srgb to the gray levels while handling an edge case of c_srgb = 1
    gray_index = static_cast<int>(std::fmin((1 - c_srgb) * gray_levels_fine_count, gray_levels_fine_count - 1));

    // Final character representing the gray level of the RGB pixel
    out[i] = gray_levels_fine[gray_index];
}

std::string
image_to_ascii(const std::string& filename, int patch_width, int patch_height)
{
    const auto char_patches = cudascii::utils::ascii_chars_to_patchs(gray_levels_fine_sv, 14);

    std::cout << "Reading file" << std::endl;

    // Load Image using CImg
    cimg_library::CImg<unsigned char> src(filename.c_str());

    std::cout << "File read successfully" << std::endl;

    // Clip image to the patch size
    src = cudascii::utils::crop_to_grid(src, patch_width, patch_height);

    // Get the cropped image dimensions
    int width{src.width()};
    int height{src.height()};

    // Perform edge detection
    src = cudascii::utils::edge_map(src);

    std::cout << std::format("Max: {}, min: {}", src.max(), src.min()) << std::endl;

    // Assess how much memory is needed for image
    const unsigned int N = width * height;
    const unsigned int bytes = N * sizeof(unsigned char);

    // Declare Host result
    std::vector<unsigned char> h_out(N, 65);  // 65 in ASCII is "A"

    // Allocate GPU memory
    unsigned char *d_out, *d_r, *d_g, *d_b;
    int cs_out, cs_r, cs_g, cs_b;
    cs_out = cudaMalloc((unsigned char**)&d_out, bytes);
    cs_r = cudaMalloc((unsigned char**)&d_r, bytes);
    cs_g = cudaMalloc((unsigned char**)&d_g, bytes);
    cs_b = cudaMalloc((unsigned char**)&d_b, bytes);

    std::cout << "Allocated GPU memory" << std::endl;

    if ((cs_out | cs_r | cs_g | cs_b) != cudaSuccess) {
        std::cout << "failed!" << std::endl;
        std::cout << cs_out << ',' << cs_r << ',' << cs_g << ',' << cs_b << std::endl;
        return "";
    }

    // Copy the image from host (CPU) to device (GPU)
    cudaMemcpy(d_r, src.channel(0), bytes, cudaMemcpyHostToDevice);
    cudaMemcpy(d_g, src.channel(1), bytes, cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, src.channel(2), bytes, cudaMemcpyHostToDevice);

    auto error = cudaGetLastError();

    std::cout << "Copied CPU to GPU memory" << std::endl;

    if (error != cudaSuccess) {
        std::cout << "cudaMemcpy" << std::endl;
        std::cout << "failure!!!!!!" << std::endl;
        std::cout << "result: " << error << std::endl;
        return "";
    }

    // Call the pixel_to_ascii code here
    // int threadsPerBlock = 256;
    // int blocksPerGrid = (N + threadsPerBlock - 1) / threadsPerBlock;

    // Launch kernel
    dim3 blockSize(16, 16);  // 16x16 threads per block
    dim3 gridSize((width + blockSize.x - 1) / blockSize.x, (height + blockSize.y - 1) / blockSize.y);

    pixel_to_ascii<<<gridSize, blockSize>>>(d_out, d_r, d_g, d_b, width, height);

    std::cout << "Changed pixel intensity to Ascii" << std::endl;

    error = cudaGetLastError();

    if (error != cudaSuccess) {
        std::cout << "image_to_ascii" << std::endl;
        std::cout << "failure!!!!!!" << std::endl;
        std::cout << "result: " << error << std::endl;
        return "";
    }

    // Copy the ascii array from device (GPU) to host (CPU)
    const auto result = cudaMemcpy(h_out.data(), d_out, bytes, cudaMemcpyDeviceToHost);

    if (result != cudaSuccess) {
        std::cout << "cudaMemcpy" << std::endl;
        std::cout << "failure!!!!!!" << std::endl;
        std::cout << "result: " << result << std::endl;
        return "";
    }

    // Don't forget to free memory!!!!
    cudaFree(d_out);
    cudaFree(d_r);
    cudaFree(d_g);
    cudaFree(d_b);

    // Build string return value
    std::string text;

    for (int row{0}; row < height; row++) {
        for (int col{0}; col < width; col++)
            text += h_out[row * width + col];

        if (row != height - 1) text += '\n';
    }

    return text;
}

}  // namespace cudascii
