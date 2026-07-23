#ifndef AUTOMATIOTEST_OPENCVLIB_MATEXTENSION_HPP
#define AUTOMATIOTEST_OPENCVLIB_MATEXTENSION_HPP

#include "../Utilities/CommonTypes.hpp"

#include <opencv2/core.hpp>

namespace automationtest::opencvlib {

class MatExtension {
public:
    static inline  std::uint8_t GetByteValue(const cv::Mat& mat, int row, int col, int channel=0)
    {
        return mat.ptr<std::uint8_t>(row)[col * mat.channels() + channel];
    }

    static void SetByteValue(cv::Mat& mat, int row, int col, std::uint8_t value, int channel = 0);
    static cv::Mat BitmapToMat(const automationtest::utilities::Bitmap& bitmap);
    static cv::Mat BitmapToGrayMat(const automationtest::utilities::Bitmap& bitmap, const automationtest::utilities::Rectangle& rect);
    static cv::Mat SafeToMat(const automationtest::utilities::Bitmap& bitmap);
    static automationtest::utilities::Bitmap ToBitmap(const cv::Mat& mat);
};

} // namespace automationtest::opencvlib

#endif // AUTOMATIOTEST_OPENCVLIB_MATEXTENSION_HPP
