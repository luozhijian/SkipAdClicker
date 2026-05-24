#ifndef AUTOMATIOTEST_OCRLIB_OPENCVEXTENSION_HPP
#define AUTOMATIOTEST_OCRLIB_OPENCVEXTENSION_HPP

#include "OcrTypes.hpp"

#include <opencv2/core.hpp>

#include <string>
#include <vector>

namespace automationtest::ocrlib {

struct PixData {
    int width {0};
    int height {0};
    int channels {1};
    int stride {0};
    std::vector<std::byte> bytes {};
};

class OpenCvExtension {
public:
    static PixData MatGrayToPix(const cv::Mat& src);
    static std::string SavePix(const PixData& pix, const std::string& filename);
};

} // namespace automationtest::ocrlib

#endif // AUTOMATIOTEST_OCRLIB_OPENCVEXTENSION_HPP
