#include "OcrProcesser.hpp"

#include "../Utilities/GlobalSetting.hpp"

#include <algorithm>
#include <cctype>
#include <memory>

#include <tesseract/baseapi.h>

#include "../Utilities/StringLib.hpp"

using automationtest::utilities::StringLib;

namespace automationtest::ocrlib {

namespace {



} // namespace

std::string OcrProcesser::TryOcrOneLineFromMat(const cv::Mat& gray)
{
    if (gray.empty() || gray.type() != CV_8UC1) {
        return {};
    }

    const auto& data_folder = utilities::GlobalSetting::tesseract_engine_data_folder;
    const auto& language = utilities::GlobalSetting::tesseract_engine_language;
    const auto* data_path = data_folder.empty() ? nullptr : data_folder.c_str();
    const auto* language_name = language.empty() ? "eng" : language.c_str();

    static tesseract::TessBaseAPI engine;
	static bool is_initialized = false;
    if (!is_initialized) {
        if (engine.Init(data_path, language_name) != 0) {
            return {};
        }
        is_initialized = true;
    }

    engine.SetPageSegMode(tesseract::PSM_SINGLE_LINE);
    engine.SetImage(gray.data, gray.cols, gray.rows, 1, static_cast<int>(gray.step));

    std::unique_ptr<char[]> text(engine.GetUTF8Text());
    return text == nullptr ? std::string {} : StringLib::Trim(text.get());
}

} // namespace automationtest::ocrlib
