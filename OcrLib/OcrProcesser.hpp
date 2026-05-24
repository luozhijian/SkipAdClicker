#ifndef AUTOMATIOTEST_OCRLIB_OCRPROCESSER_HPP
#define AUTOMATIOTEST_OCRLIB_OCRPROCESSER_HPP

#include "OcrTypes.hpp"

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace automationtest::ocrlib {

class OcrProcesser {
public:
    static void ForceLoad();
    static void RegisterEngine(const std::string& language, std::shared_ptr<IOcrEngine> engine);
    static std::shared_ptr<IOcrEngine> GetEngine(const std::string& default_language = "eng");

    static std::string TryOcrMultipleLinesOrOneLine(bool multiple_lines, const cv::Mat& gray, const std::string& target_string = {}, double zoom_scale = 1.0, const std::string& default_language = "eng");
    static std::string TryOcrMultipleLinesOrOneLine(bool multiple_lines, const Bitmap& bitmap, const std::string& target_string = {}, double zoom_scale = 1.0, const std::string& default_language = "eng");
    static std::string TryOcrMultipleLines(const Bitmap& bitmap, const std::string& target_string = {}, double zoom_scale = 1.0, const std::string& default_language = "eng");
    static std::string TryOcr(const Bitmap& bitmap, const std::string& target_string = {}, double zoom_scale = 1.0, const std::string& default_language = "eng");
    static std::string TryOcrOneLineFromMat(const cv::Mat& gray, const std::string& target_string = {}, double zoom_scale_min = 1.0, double zoom_scale_max = 1.0, const std::string& default_language = "eng");
    static std::string TryOcrOneLine(const Bitmap& bitmap, const std::string& target_string = {}, double zoom_scale_min = 1.0, double zoom_scale_max = 1.0, const std::string& default_language = "eng");
    static std::string TryOcrOneLineOne(const Bitmap& bitmap, float& confidence, double zoom_scale = 1.0, const std::string& target_string = {}, const std::string& default_language = "eng");

private:
    static Bitmap ScaleBitmap(const Bitmap& bitmap, double zoom_scale);
    static std::shared_ptr<IOcrEngine> BuildDefaultEngine(const std::string& default_language);

    static std::unordered_map<std::string, std::shared_ptr<IOcrEngine>> engines_;
    static std::mutex sync_object_;
};

} // namespace automationtest::ocrlib

#endif // AUTOMATIOTEST_OCRLIB_OCRPROCESSER_HPP
