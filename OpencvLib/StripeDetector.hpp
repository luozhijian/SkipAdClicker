#ifndef AUTOMATIOTEST_OPENCVLIB_STRIPEDETECTOR_HPP
#define AUTOMATIOTEST_OPENCVLIB_STRIPEDETECTOR_HPP

#include <opencv2/core.hpp>

#include "../Utilities/CommonTypes.hpp"

#include <vector>

namespace automationtest::opencvlib {

class StripeDetector {
public:
    static bool DetectIfStripExists(const cv::Mat& cannied, int kernel_w = 400, int kernel_h = 7);
    static std::vector<automationtest::utilities::Rectangle> Detect(const cv::Mat& cannied, int kernel_w = 400, int kernel_h = 7, int min_area = 10);

private:
    static cv::Mat ErodeInverted(const cv::Mat& cannied, int kernel_w, int kernel_h);
    static bool FindAnyStripArea(const cv::Mat& binary);
    static std::vector<automationtest::utilities::Rectangle> FindMaxRectanglesOfWhiteArea(const cv::Mat& binary, int min_area);
    static std::vector<automationtest::utilities::Rectangle> RemoveContainedRectangles(std::vector<automationtest::utilities::Rectangle> candidates);
};

} // namespace automationtest::opencvlib

#endif // AUTOMATIOTEST_OPENCVLIB_STRIPEDETECTOR_HPP
