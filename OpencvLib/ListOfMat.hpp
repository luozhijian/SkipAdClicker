#ifndef AUTOMATIOTEST_OPENCVLIB_LISTOFMAT_HPP
#define AUTOMATIOTEST_OPENCVLIB_LISTOFMAT_HPP

#include <opencv2/core.hpp>

#include <cstddef>
#include <optional>
#include <vector>

namespace automationtest::opencvlib {

class ListOfMat {
public:
    std::vector<cv::Mat> mats {};
    std::optional<std::size_t> last_used {};

    [[nodiscard]] std::vector<const cv::Mat*> Ordered() const;
};

} // namespace automationtest::opencvlib

#endif // AUTOMATIOTEST_OPENCVLIB_LISTOFMAT_HPP
