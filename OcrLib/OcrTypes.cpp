#include "OcrTypes.hpp"

#include "../OpencvLib/MatExtension.hpp"

namespace automationtest::ocrlib {

OcrResult IOcrEngine::ProcessMat(const cv::Mat& gray, bool single_line, const std::string& whitelist)
{
    return ProcessBitmap(automationtest::opencvlib::MatExtension::ToBitmap(gray), single_line, whitelist);
}

CallbackOcrEngine::CallbackOcrEngine(BitmapProcessor processor)
    : processor_(std::move(processor))
{
}

OcrResult CallbackOcrEngine::ProcessBitmap(const Bitmap& bitmap, bool single_line, const std::string& whitelist)
{
    if (!processor_) {
        return {};
    }
    return processor_(bitmap, single_line, whitelist);
}

} // namespace automationtest::ocrlib
