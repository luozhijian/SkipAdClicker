#include "OpenCvExtension.hpp"

#include <cstring>
#include <fstream>

namespace automationtest::ocrlib {

PixData OpenCvExtension::MatGrayToPix(const cv::Mat& src)
{
    const auto contiguous = src.isContinuous() ? src : src.clone();
    PixData pix {};
    pix.width = contiguous.cols;
    pix.height = contiguous.rows;
    pix.channels = contiguous.channels();
    pix.stride = static_cast<int>(contiguous.step);
    const auto byte_count = contiguous.total() * contiguous.elemSize();
    pix.bytes.resize(byte_count);
    std::memcpy(pix.bytes.data(), contiguous.data, byte_count);
    return pix;
}

std::string OpenCvExtension::SavePix(const PixData& pix, const std::string& filename)
{
    if (filename.empty() || pix.bytes.empty()) {
        return {};
    }

    std::ofstream output(filename, std::ios::binary);
    if (!output) {
        return {};
    }

    output.write(reinterpret_cast<const char*>(pix.bytes.data()), static_cast<std::streamsize>(pix.bytes.size()));
    return filename;
}

} // namespace automationtest::ocrlib
