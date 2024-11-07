// used to silence compiler warnings for this 3rd part library
#pragma GCC system_header

#ifndef CIMG_CIMG
#    define CIMG_CIMG

// https://stackoverflow.com/questions/38903996/cimg-x11-linking-error-using-clion-and-cmake
// we are using CImg headless
#    define cimg_display 0
#    define cimg_use_png 1

#    include <CImg.h>

#endif