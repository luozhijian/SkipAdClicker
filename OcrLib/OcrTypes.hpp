#ifndef AUTOMATIOTEST_OCRLIB_OCRTYPES_HPP
#define AUTOMATIOTEST_OCRLIB_OCRTYPES_HPP

#include "../Utilities/CommonTypes.hpp"
#include <opencv2/core.hpp>

#include <functional>
#include <memory>
#include <optional>
#include <string>

namespace automationtest::ocrlib {

using Bitmap = automationtest::utilities::Bitmap;
struct OcrResult {
    std::string text {};
    float confidence {0.0F};
};

class IOcrEngine {
public:
    virtual ~IOcrEngine() = default;
    virtual OcrResult ProcessBitmap(const Bitmap& bitmap, bool single_line, const std::string& whitelist) = 0;
    virtual OcrResult ProcessMat(const cv::Mat& gray, bool single_line, const std::string& whitelist);
};

class CallbackOcrEngine final : public IOcrEngine {
public:
    using BitmapProcessor = std::function<OcrResult(const Bitmap&, bool, const std::string&)>;

    explicit CallbackOcrEngine(BitmapProcessor processor);
    OcrResult ProcessBitmap(const Bitmap& bitmap, bool single_line, const std::string& whitelist) override;

private:
    BitmapProcessor processor_ {};
};

} // namespace automationtest::ocrlib

#endif // AUTOMATIOTEST_OCRLIB_OCRTYPES_HPP
