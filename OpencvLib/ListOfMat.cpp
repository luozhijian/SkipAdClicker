#include "ListOfMat.hpp"

namespace automationtest::opencvlib {

std::vector<const cv::Mat*> ListOfMat::Ordered() const
{
    std::vector<const cv::Mat*> ordered {};
    ordered.reserve(mats.size());
    if (last_used.has_value() && last_used.value() < mats.size()) {
        ordered.push_back(&mats[last_used.value()]);
    }

    for (std::size_t index = 0; index < mats.size(); ++index) {
        if (last_used.has_value() && index == last_used.value()) {
            continue;
        }

        ordered.push_back(&mats[index]);
    }

    return ordered;
}

} // namespace automationtest::opencvlib
