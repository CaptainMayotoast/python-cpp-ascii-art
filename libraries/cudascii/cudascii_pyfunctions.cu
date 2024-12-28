#include <cuda_runtime.h>

#include <chrono>
#include <cmath>
#include <iostream>
#include <optional>
#include <string>
#include <thread>

#include <cimg_cimg.hpp>
#include <cudascii_utils.hpp>
#include <ScopeGuard.hpp>

#include <spdlog/spdlog.h>

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

    for (int reference_char_index = 0; reference_char_index < gray_levels_fine_count;
         reference_char_index++)
    {
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

__host__ __device__ void
pixel_to_ascii(unsigned char* out, unsigned char* r, unsigned char* g, unsigned char* b, int i)
{
    // Standard linear combination
    const float c_linear{
            RED_WEIGHT * (r[i] / 255.0f) + GREEN_WEIGHT * (g[i] / 255.0f)
            + BLUE_WEIGHT * (b[i] / 255.0f)};

    float c_srgb{0.0f};

    // If gray level is very dark, use linear scaling
    if (c_linear <= CONVERSION_THRESHOLD) {
        c_srgb = BELOW_THRESHOLD_SCALAR * c_linear;
    } else {
        // Non linear scaling to adjust for gamma exposure
        c_srgb = ABOVE_THRESHOLD_SCALAR * powf(c_linear, ABOVE_THRESHOLD_EXPONENT)
                 + ABOVE_THRESHOLD_OFFSET;
    }

    // Scale c_srgb to the gray levels while handling an edge case of c_srgb = 1
    const int gray_index = static_cast<int>(
            std::fmin((1 - c_srgb) * gray_levels_fine_count, gray_levels_fine_count - 1));

    // Final character representing the gray level of the RGB pixel
    out[i] = gray_levels_fine[gray_index];
}

__global__ void
pixel_to_ascii_kernel(
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

    pixel_to_ascii(out, r, g, b, i);
}

// __global__ void
// generate_ascii_from_patch_comparison_kernel(
//         unsigned char* out,
//         unsigned char* r,
//         unsigned char* g,
//         unsigned char* b,
//         int width,
//         int height)
// {
// }

std::string
image_to_ascii(const std::string& filename, int patch_width, int patch_height)
{
    spdlog::set_level(spdlog::level::debug);

    spdlog::info("Reading file: {}", filename);

    const auto char_patches = cudascii::utils::ascii_chars_to_patchs(gray_levels_fine_sv, 14);

    std::optional<cimg_library::CImg<unsigned char>> src;

    // Load Image using CImg
    try {
        src = std::make_optional<cimg_library::CImg<unsigned char>>(filename.c_str());
        spdlog::info("File read successfully");
    } catch (const std::exception& e) {
        spdlog::error("Unable to create a CImg object from {} with error {}", filename, e.what());
        return "";
    } catch (...) {
        spdlog::error("Unable to create a CImg object from {} with unknown error", filename);
        return "";
    }

    // Clip image to the patch size
    src = cudascii::utils::crop_to_grid(*src, patch_width, patch_height);

    if (src->is_empty()) {
        spdlog::error("Crop to grid result is empty");
        return "";
    }

    // Perform edge detection
    src = cudascii::utils::edge_map(*src);

    if (src->is_empty()) {
        spdlog::error("Edge map result is empty");
        return "";
    }

    spdlog::info("Max: {}, min: {}", src->max(), src->min());

    // Get the cropped image dimensions
    const int width{src->width()};
    const int height{src->height()};

    // Assess how much memory is needed for image
    const unsigned int N = width * height;
    const unsigned int bytes = N * sizeof(unsigned char);

    // Declare Host result
    std::vector<unsigned char> h_out(N, 65);  // 65 in ASCII is "A"

    // Allocate GPU memory
    unsigned char* d_out{nullptr};
    unsigned char* d_r{nullptr};
    unsigned char* d_g{nullptr};
    unsigned char* d_b{nullptr};

    const auto cs_out{cudaMalloc(static_cast<unsigned char**>(&d_out), bytes)};

    if (cs_out != cudaSuccess) {
        spdlog::error("failed! cs_out {}", cudaGetErrorString(cs_out));
        return "";
    }

    finally
    {
        spdlog::debug("Freeing CUDA memory for cs_out");

        cudaFree(d_out);
    };

    const auto cs_r{cudaMalloc(static_cast<unsigned char**>(&d_r), bytes)};

    if (cs_r != cudaSuccess) {
        spdlog::error("failed! cs_r {}", cudaGetErrorString(cs_r));
        return "";
    }

    finally
    {
        spdlog::debug("Freeing CUDA memory for cs_r");

        cudaFree(d_r);
    };

    const auto cs_g{cudaMalloc(static_cast<unsigned char**>(&d_g), bytes)};

    if (cs_g != cudaSuccess) {
        spdlog::error("failed! cs_g {}", cudaGetErrorString(cs_g));
        return "";
    }

    finally
    {
        spdlog::debug("Freeing CUDA memory for cs_g");

        cudaFree(d_g);
    };

    const auto cs_b{cudaMalloc(static_cast<unsigned char**>(&d_b), bytes)};

    if (cs_b != cudaSuccess) {
        spdlog::error("failed! cs_b {}", cudaGetErrorString(cs_b));
        return "";
    }

    finally
    {
        spdlog::debug("Freeing CUDA memory for cs_b");

        cudaFree(d_b);
    };

    spdlog::info("Allocated GPU memory");

    // Copy the image from host (CPU) to device (GPU)
    cudaMemcpy(d_r, src->channel(0), bytes, cudaMemcpyHostToDevice);
    cudaMemcpy(d_g, src->channel(1), bytes, cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, src->channel(2), bytes, cudaMemcpyHostToDevice);

    auto error = cudaGetLastError();

    spdlog::info("Copied CPU to GPU memory");

    if (error != cudaSuccess) {
        spdlog::error("cudaMemcpy failure with result {}", std::to_string(error));
        return "";
    }

    dim3 blockSize(16, 16);  // 16x16 threads per block
    dim3 gridSize((width + blockSize.x - 1) / blockSize.x, (height + blockSize.y - 1) / blockSize.y);

    pixel_to_ascii_kernel<<<gridSize, blockSize>>>(d_out, d_r, d_g, d_b, width, height);

    spdlog::info("Changed pixel intensity to Ascii");

    error = cudaGetLastError();

    if (error != cudaSuccess) {
        spdlog::error("cudaMemcpy failure with result {}", cudaGetErrorString(error));
        return "";
    }

    // Copy the ascii array from device (GPU) to host (CPU)
    const auto result = cudaMemcpy(h_out.data(), d_out, bytes, cudaMemcpyDeviceToHost);

    if (result != cudaSuccess) {
        spdlog::error("cudaMemcpy failure with result {}", cudaGetErrorString(error));
        return "";
    }

    return cudascii::utils::buildString(h_out, width, height);
}

}  // namespace cudascii::pyfunctions
