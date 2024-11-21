#include <ranges>
#include <vector>

namespace ranges_helpers {

/// @brief an implementation of std::ranges::to
/// @note https://stackoverflow.com/questions/63115900/c20-ranges-view-to-vector
/// @tparam R the range type
/// @param r the actual range
/// @return the vector of the type provided
template<std::ranges::range R>
auto
to_vector(R&& r)
{
    auto r_common = r | std::views::common;
    return std::vector(r_common.begin(), r_common.end());
}

}  // namespace ranges_helpers
