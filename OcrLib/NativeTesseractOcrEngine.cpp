#include "NativeTesseractOcrEngine.hpp"

#include "../Utilities/BitmapHelper.hpp"

#include <cstdlib>
#include <memory>
#include <string>

#if AUTOMATIOTEST_HAS_TESSERACT
#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>
#endif

namespace automationtest::ocrlib {

using automationtest::utilities::BitmapHelper;

#if AUTOMATIOTEST_HAS_TESSERACT
namespace {

Pix* BitmapToPix(const Bitmap& source)
{
    const auto gray = source.channels == 1 ? source : BitmapHelper::ConvertToGrayscale(source);
    auto* pix = pixCreate(gray.width, gray.height, 8);
    if (pix == nullptr) {
        return nullptr;
    }

    auto* data = pixGetData(pix);
    const auto words_per_line = pixGetWpl(pix);
    for (int y = 0; y < gray.height; ++y) {
        auto* row = data + static_cast<std::size_t>(y) * static_cast<std::size_t>(words_per_line);
        for (int x = 0; x < gray.width; ++x) {
            const auto index = static_cast<std::size_t>(y) * static_cast<std::size_t>(gray.stride) + static_cast<std::size_t>(x);
            const auto value = static_cast<l_uint32>(static_cast<unsigned char>(gray.pixels[index]));
            SET_DATA_BYTE(row, x, value);
        }
    }

    return pix;
}

int ResolvePageSegMode(bool single_line)
{
    return single_line ? static_cast<int>(tesseract::PageSegMode::PSM_SINGLE_LINE)
                       : static_cast<int>(tesseract::PageSegMode::PSM_AUTO);
}

} // namespace
#endif

NativeTesseractOcrEngine::NativeTesseractOcrEngine(std::string language)
    : language_(std::move(language))
{
}

NativeTesseractOcrEngine::~NativeTesseractOcrEngine() = default;

OcrResult NativeTesseractOcrEngine::ProcessBitmap(const Bitmap& bitmap, bool single_line, const std::string& whitelist)
{
#if AUTOMATIOTEST_HAS_TESSERACT
    auto pix_deleter = [](Pix* pix) {
        if (pix != nullptr) {
            pixDestroy(&pix);
        }
    };

    tesseract::TessBaseAPI api {};
    if (api.Init(nullptr, language_.c_str()) != 0) {
        return {};
    }

    api.SetPageSegMode(static_cast<tesseract::PageSegMode>(ResolvePageSegMode(single_line)));
    if (!whitelist.empty()) {
        api.SetVariable("tessedit_char_whitelist", whitelist.c_str());
    }

    std::unique_ptr<Pix, decltype(pix_deleter)> pix(BitmapToPix(bitmap), pix_deleter);
    if (!pix) {
        api.End();
        return {};
    }

    api.SetImage(pix.get());
    std::unique_ptr<char, decltype(&std::free)> text(api.GetUTF8Text(), &std::free);

    OcrResult result {};
    if (text) {
        result.text = text.get();
    }
    result.confidence = api.MeanTextConf() / 100.0F;
    api.End();
    return result;
#else
    (void)bitmap;
    (void)single_line;
    (void)whitelist;
    return {};
#endif
}

std::shared_ptr<IOcrEngine> TryCreateNativeTesseractEngine(const std::string& language)
{
#if AUTOMATIOTEST_HAS_TESSERACT
    return std::make_shared<NativeTesseractOcrEngine>(language);
#else
    (void)language;
    return {};
#endif
}

} // namespace automationtest::ocrlib
