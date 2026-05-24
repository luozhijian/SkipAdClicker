#ifndef AUTOMATIOTEST_OCRLIB_NATIVETESSERACTOCRENGINE_HPP
#define AUTOMATIOTEST_OCRLIB_NATIVETESSERACTOCRENGINE_HPP

#include "OcrTypes.hpp"

#include <memory>
#include <string>

namespace automationtest::ocrlib {

class NativeTesseractOcrEngine final : public IOcrEngine {
public:
    explicit NativeTesseractOcrEngine(std::string language);
    ~NativeTesseractOcrEngine() override;

    NativeTesseractOcrEngine(const NativeTesseractOcrEngine&) = delete;
    NativeTesseractOcrEngine& operator=(const NativeTesseractOcrEngine&) = delete;

    OcrResult ProcessBitmap(const Bitmap& bitmap, bool single_line, const std::string& whitelist) override;

private:
    std::string language_ {};
};

std::shared_ptr<IOcrEngine> TryCreateNativeTesseractEngine(const std::string& language);

} // namespace automationtest::ocrlib

#endif // AUTOMATIOTEST_OCRLIB_NATIVETESSERACTOCRENGINE_HPP
