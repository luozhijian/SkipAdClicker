#ifndef AUTOMATIOTEST_OCRLIB_OCRPROCESSER_HPP
#define AUTOMATIOTEST_OCRLIB_OCRPROCESSER_HPP

#include <opencv2/core/mat.hpp>

#include <string>

namespace automationtest::ocrlib {

class OcrProcesser {
public:
    static std::string TryOcrOneLineFromMat(const cv::Mat& gray);
};

} // namespace automationtest::ocrlib

#endif // AUTOMATIOTEST_OCRLIB_OCRPROCESSER_HPP
