#include "MatExtension.hpp"

#include "../Utilities/BitmapHelper.hpp"

#include <algorithm>
#include <cstring>

namespace automationtest::opencvlib {

namespace {

int OpenCvTypeForChannels(int channels)
{
    return CV_MAKETYPE(CV_8U, std::max(1, channels));
}

} // namespace


void MatExtension::SetByteValue(cv::Mat& mat, int row, int col, std::uint8_t value, int channel)
{
    mat.ptr<std::uint8_t>(row)[col * mat.channels() + channel] = value;
}

cv::Mat MatExtension::BitmapToMat(const automationtest::utilities::Bitmap& bitmap)
{
    if (bitmap.width <= 0 || bitmap.height <= 0 || bitmap.pixels.empty()) {
        return {};
    }

    const int channels = bitmap.channels <= 0 ? 1 : bitmap.channels;
    const auto stride = static_cast<std::size_t>(bitmap.stride > 0 ? bitmap.stride : bitmap.width * channels);
    const cv::Mat view(bitmap.height, bitmap.width, OpenCvTypeForChannels(channels), const_cast<std::byte*>(bitmap.pixels.data()), stride);
    return view.clone();
}

cv::Mat MatExtension::BitmapToGrayMat(const automationtest::utilities::Bitmap& bitmap, const automationtest::utilities::Rectangle& rect)
{
    return SafeToMat(automationtest::utilities::BitmapHelper::GetBitmap(bitmap, rect));
}

cv::Mat MatExtension::SafeToMat(const automationtest::utilities::Bitmap& bitmap)
{
    auto gray = automationtest::utilities::BitmapHelper::ConvertToGrayscale(bitmap);
    return BitmapToMat(gray);
}

automationtest::utilities::Bitmap MatExtension::ToBitmap(const cv::Mat& mat)
{
    automationtest::utilities::Bitmap bitmap {};
    if (mat.empty()) {
        return bitmap;
    }

    const auto contiguous = mat.isContinuous() ? mat : mat.clone();
    bitmap.width = contiguous.cols;
    bitmap.height = contiguous.rows;
    bitmap.channels = contiguous.channels();
    bitmap.stride = static_cast<int>(contiguous.step);
    const auto byte_count = contiguous.total() * contiguous.elemSize();
    bitmap.pixels.resize(byte_count);
    std::memcpy(bitmap.pixels.data(), contiguous.data, byte_count);
    return bitmap;
}

} // namespace automationtest::opencvlib
