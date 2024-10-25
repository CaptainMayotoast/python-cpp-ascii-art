#ifndef SAMPLE_SAMPLE
#define SAMPLE_SAMPLE

#include <cstdint>
#include <iostream>
#include <optional>

class SDL_Surface;

namespace sample {

class Sample
{
  public:
    Sample();

    [[nodiscard]] std::optional<std::uint32_t> getPixel(SDL_Surface* surface, int x, int y);
};

}  // namespace sample

#endif