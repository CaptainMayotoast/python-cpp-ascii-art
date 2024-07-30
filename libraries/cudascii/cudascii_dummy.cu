#include <cuda_runtime.h>

#include <chrono>
#include <cmath>
#include <iostream>
#include <string>
#include <thread>
#include <cimg_cimg.hpp>

namespace {
    // Algorithm Parameterization
    // const std::string gray_levels_fine =
    // "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~i!lI;:,\"^`. ";
    const constexpr char* gray_level_lookup{"@%#*+=-:. "};
    const constexpr int gray_levels = 10;
    const constexpr float RED_WEIGHT = 0.2126;
    const constexpr float GREEN_WEIGHT = 0.7152;
    const constexpr float BLUE_WEIGHT = 0.0722;
    const constexpr float CONVERSION_THRESHOLD = 0.0031308;
    const constexpr float BELOW_THRESHOLD_SCALAR = 12.92;
    const constexpr float ABOVE_THRESHOLD_SCALAR = 1.055;
    const constexpr float ABOVE_THRESHOLD_EXPONENT = 1 / 2.4;
    const constexpr float ABOVE_THRESHOLD_OFFSET = -0.055;
}

namespace cudascii {

    __global__ void setPixelsTo255(unsigned char *out, int width, int height) {
        // Calculate the global pixel index
        int x = blockIdx.x * blockDim.x + threadIdx.x;
        int y = blockIdx.y * blockDim.y + threadIdx.y;
        
        // Check if within bounds
        if (x < width && y < height) {
            // Calculate 1D index for the pixel
            int index = y * width + x;
            
            // Set pixel value to 255 (white)
            out[index] = 66;
        }
    }


    __global__ void pixel_to_ascii(unsigned char *out, unsigned char *r, unsigned char *g, unsigned char *b, int width, int height) {

        // Calculate the global pixel index
        int x = blockIdx.x * blockDim.x + threadIdx.x;
        int y = blockIdx.y * blockDim.y + threadIdx.y;

        if (!(x < width && y < height))
            return;

        int i = y * width + x;
        // Thread index
        // int i = threadIdx.x + blockIdx.x * blockDim.x;

        float c_linear, c_srgb;
        int gray_index{0};
        
        // Standard linear combination
        c_linear = RED_WEIGHT*(r[i]/255.) + GREEN_WEIGHT*(g[i]/255.) + BLUE_WEIGHT*(b[i]/255.);
        
        // If gray level is very dark, use linear scaling
        if (c_linear <= CONVERSION_THRESHOLD)
            c_srgb = BELOW_THRESHOLD_SCALAR * c_linear;

        // Non linear scaling to adjust for gamma exposure
        else
            c_srgb = ABOVE_THRESHOLD_SCALAR * powf(c_linear,ABOVE_THRESHOLD_EXPONENT) + ABOVE_THRESHOLD_OFFSET;
        
        // Scale c_srgb to the gray levels while handling an edge case of c_srgb = 1
        gray_index = static_cast<int>(fmin((1-c_srgb) * gray_levels, gray_levels - 1));

        // Final character representing the gray level of the RGB pixel
        out[i] = gray_level_lookup[gray_index];
    }

    std::string image_to_ascii(const std::string &filename, int patch_width, int patch_height) {

        std::cout << "Reading file" << std::endl;

        // Load Image using CImg
        cimg_library::CImg<unsigned char> src(filename.c_str());

        std::cout << "File read successfully" << std::endl;

        // Get the image dimensions
        int width{src.width()};
        int height{src.height()};

        // clip image to the patch size
        width = (width / patch_width) * patch_width;
        height = (height / patch_height) * patch_height;

        src = src.resize(width, height);

        // this is edge detection
        // https://en.wikipedia.org/wiki/Image_gradient
        // https://cimg.eu/reference/structcimg__library_1_1CImg.html#a6c7b2bc4442e062706fa1bbc04621d8f
        auto gradientList = src.get_gradient("xy", 0);

        // get_gradient returns two elements here, gradient in the north-south direction and in the east-west direction
        // abs is taken because we do not care about negative values, sign (+/-) indicates a side, but we do not care about which side the gradient value is on)
        // gray is no gradient, black would be negative, white is positive
        src = cimg_library::CImg<unsigned char>(gradientList.at(0).abs() + gradientList.at(1).abs());

        std::cout << std::format("Max: {}, min: {}", src.max(), src.min()) << std::endl;

        // Assess how much memory is needed for image
        const unsigned int N = width*height;
        const unsigned int bytes = N * sizeof(unsigned char);

        // Declare Host result
        std::vector<unsigned char> h_out(N, 65); // 65 in ASCII is "A"

        // Allocate GPU memory
        unsigned char *d_out, *d_r, *d_g, *d_b;
        int cs_out, cs_r, cs_g, cs_b;
        cs_out = cudaMalloc((unsigned char**)&d_out, bytes);
        cs_r = cudaMalloc((unsigned char**)&d_r, bytes);
        cs_g = cudaMalloc((unsigned char**)&d_g, bytes);
        cs_b = cudaMalloc((unsigned char**)&d_b, bytes);

        std::cout << "Allocated GPU memory" << std::endl;
        
        if((cs_out | cs_r | cs_g | cs_b) != cudaSuccess)
        {
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

        if(error != cudaSuccess)
        {
            std::cout << "cudaMemcpy" << std::endl;
            std::cout << "failure!!!!!!" << std::endl; 
            std::cout << "result: " << error << std::endl; 
            return "";
        }
        
        // Call the pixel_to_ascii code here
        int threadsPerBlock = 256;
        int blocksPerGrid = (N + threadsPerBlock - 1) / threadsPerBlock;

        // Launch kernel
        dim3 blockSize(16, 16); // 16x16 threads per block
        dim3 gridSize((width + blockSize.x - 1) / blockSize.x, (height + blockSize.y - 1) / blockSize.y);

        pixel_to_ascii<<<gridSize, blockSize>>>(d_out, d_r, d_g, d_b, width, height);

        std::cout << "Changed pixel intensity to Ascii" << std::endl;

        error = cudaGetLastError();

        if(error != cudaSuccess)
        {
            std::cout << "image_to_ascii" << std::endl;
            std::cout << "failure!!!!!!" << std::endl; 
            std::cout << "result: " << error << std::endl; 
            return "";
        }

        // Copy the ascii array from device (GPU) to host (CPU)
        const auto result = cudaMemcpy(h_out.data(), d_out, bytes, cudaMemcpyDeviceToHost);

        if(result != cudaSuccess)
        {
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
                text += h_out[row*width + col];                

            if (row != height-1)
                text += '\n';

        }

        return text;

    }

    bool test_cuda() {

        // Assess how much memory is needed for image
        unsigned int width, height;
        width = 2560;
        height = 5120;

        const unsigned int N = width * height;
        const unsigned int bytes = N * sizeof(unsigned char);

        // Allocate GPU memory
        unsigned char *d_a;
        if(cudaMalloc((unsigned char**)&d_a, bytes) != cudaSuccess)
        {
            std::cout << "failed!" << std::endl;
            return false;
        }
        else
        {
            std::cout << "passed!" << std::endl;
        }

        dim3 blockSize(16, 16); // 16x16 threads per block
        dim3 gridSize((width + blockSize.x - 1) / blockSize.x, (height + blockSize.y - 1) / blockSize.y);

        setPixelsTo255<<<gridSize, blockSize>>>(d_a, width, height);

        const auto error = cudaGetLastError();

        if(error != cudaSuccess)
        {
            std::cout << "failure!!!!!!" << std::endl; 
            std::cout << "result: " << error << std::endl; 
            return "";
        }

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(3s);

        // Don't forget to free memory!!!!
        cudaFree(d_a);
        
        return true;

    }
}