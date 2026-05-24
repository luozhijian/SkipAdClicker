#ifndef AUTOMATIOTEST_OPENCVLIB_CORNERDETECTOR_HPP
#define AUTOMATIOTEST_OPENCVLIB_CORNERDETECTOR_HPP

#include "../Utilities/CommonTypes.hpp"

#include <vector>

namespace automationtest::opencvlib {

class CornerDetector {
public:
    std::vector<automationtest::utilities::Point> Detect(const std::vector<automationtest::utilities::Point>& points) const;
};

} // namespace automationtest::opencvlib

#endif // AUTOMATIOTEST_OPENCVLIB_CORNERDETECTOR_HPP
