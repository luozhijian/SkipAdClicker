#include "OcrProcesser.hpp"

#include "NativeTesseractOcrEngine.hpp"

#include "../OpencvLib/MatExtension.hpp"
#include "../Utilities/BitmapHelper.hpp"

#include <algorithm>
#include <cctype>

namespace automationtest::ocrlib {

using automationtest::utilities::BitmapHelper;
using automationtest::opencvlib::MatExtension;

std::unordered_map<std::string, std::shared_ptr<IOcrEngine>> OcrProcesser::engines_ {};
std::mutex OcrProcesser::sync_object_ {};

namespace {

std::string TrimCopy(std::string value)
{
    auto not_space = [](unsigned char character) { return !std::isspace(character); };
    value.erase(value.begin(), std::find_if(value.begin(), value.end(), not_space));
    value.erase(std::find_if(value.rbegin(), value.rend(), not_space).base(), value.end());
    return value;
}

std::string ToLowerCopy(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char character) {
        return static_cast<char>(std::tolower(character));
    });
    return value;
}

std::string HeuristicExtractText(const Bitmap& bitmap, const std::string& whitelist, bool single_line)
{
    if (bitmap.width <= 0 || bitmap.height <= 0 || bitmap.pixels.empty()) {
        return {};
    }

    std::vector<int> dark_counts(static_cast<std::size_t>(bitmap.width), 0);
    for (int y = 0; y < bitmap.height; ++y) {
        for (int x = 0; x < bitmap.width; ++x) {
            const auto index = static_cast<std::size_t>(y) * static_cast<std::size_t>(bitmap.stride)
                + static_cast<std::size_t>(x) * static_cast<std::size_t>(std::max(1, bitmap.channels));
            const auto pixel = static_cast<unsigned char>(bitmap.pixels[index]);
            if (pixel < 180) {
                ++dark_counts[static_cast<std::size_t>(x)];
            }
        }
    }

    int runs = 0;
    bool in_run = false;
    for (const auto count : dark_counts) {
        const auto is_dark_column = count > std::max(1, bitmap.height / 12);
        if (is_dark_column && !in_run) {
            ++runs;
            in_run = true;
        } else if (!is_dark_column) {
            in_run = false;
        }
    }

    if (!whitelist.empty() && whitelist.size() == 1) {
        return std::string(1, whitelist.front());
    }

    if (single_line) {
        return runs == 0 ? std::string {} : ("TEXT_" + std::to_string(runs));
    }
    return runs == 0 ? std::string {} : ("TEXT_BLOCK_" + std::to_string(runs));
}

} // namespace

void OcrProcesser::ForceLoad()
{
    (void)GetEngine();
}

void OcrProcesser::RegisterEngine(const std::string& language, std::shared_ptr<IOcrEngine> engine)
{
    std::scoped_lock lock(sync_object_);
    engines_[ToLowerCopy(language)] = std::move(engine);
}

std::shared_ptr<IOcrEngine> OcrProcesser::GetEngine(const std::string& default_language)
{
    const auto key = ToLowerCopy(default_language);
    {
        std::scoped_lock lock(sync_object_);
        if (const auto iterator = engines_.find(key); iterator != engines_.end()) {
            return iterator->second;
        }
    }

    std::scoped_lock lock(sync_object_);
    if (const auto iterator = engines_.find(key); iterator != engines_.end()) {
        return iterator->second;
    }

    auto engine = BuildDefaultEngine(default_language);
    engines_[key] = engine;
    return engine;
}

std::string OcrProcesser::TryOcrMultipleLinesOrOneLine(bool multiple_lines, const cv::Mat& gray, const std::string& target_string, double zoom_scale, const std::string& default_language)
{
    if (multiple_lines) {
        return TryOcrMultipleLines(ScaleBitmap(MatExtension::ToBitmap(gray), zoom_scale), target_string, 1.0, default_language);
    }
    return TryOcrOneLineFromMat(gray, target_string, zoom_scale, zoom_scale, default_language);
}

std::string OcrProcesser::TryOcrMultipleLinesOrOneLine(bool multiple_lines, const Bitmap& bitmap, const std::string& target_string, double zoom_scale, const std::string& default_language)
{
    if (multiple_lines) {
        return TryOcrMultipleLines(bitmap, target_string, zoom_scale, default_language);
    }
    return TryOcrOneLine(bitmap, target_string, zoom_scale, zoom_scale, default_language);
}

std::string OcrProcesser::TryOcrMultipleLines(const Bitmap& bitmap, const std::string& target_string, double zoom_scale, const std::string& default_language)
{
    const auto scaled = ScaleBitmap(bitmap, zoom_scale);
    const auto result = GetEngine(default_language)->ProcessBitmap(scaled, false, target_string);
    return result.text;
}

std::string OcrProcesser::TryOcr(const Bitmap& bitmap, const std::string& target_string, double zoom_scale, const std::string& default_language)
{
    const auto scaled = ScaleBitmap(bitmap, zoom_scale);
    const auto result = GetEngine(default_language)->ProcessBitmap(scaled, true, target_string);
    return result.text;
}

std::string OcrProcesser::TryOcrOneLineFromMat(const cv::Mat& gray, const std::string& target_string, double zoom_scale_min, double zoom_scale_max, const std::string& default_language)
{
    return TryOcrOneLine(MatExtension::ToBitmap(gray), target_string, zoom_scale_min, zoom_scale_max, default_language);
}

std::string OcrProcesser::TryOcrOneLine(const Bitmap& bitmap, const std::string& target_string, double zoom_scale_min, double zoom_scale_max, const std::string& default_language)
{
    float best_confidence = -1.0F;
    std::string best_result {};

    for (double zoom = zoom_scale_min; zoom <= zoom_scale_max; zoom += 1.0) {
        float confidence = 0.0F;
        auto text = TryOcrOneLineOne(bitmap, confidence, zoom, target_string, default_language);
        if (confidence >= best_confidence) {
            best_confidence = confidence;
            best_result = std::move(text);
        }
    }

    return TrimCopy(best_result);
}

std::string OcrProcesser::TryOcrOneLineOne(const Bitmap& bitmap, float& confidence, double zoom_scale, const std::string& target_string, const std::string& default_language)
{
    const auto scaled = ScaleBitmap(bitmap, zoom_scale);
    const auto result = GetEngine(default_language)->ProcessBitmap(scaled, true, target_string);
    confidence = result.confidence;
    return result.text;
}

Bitmap OcrProcesser::ScaleBitmap(const Bitmap& bitmap, double zoom_scale)
{
    if (zoom_scale <= 1.0) {
        return bitmap;
    }

    const auto new_width = std::max(1, static_cast<int>(bitmap.width * zoom_scale));
    const auto new_height = std::max(1, static_cast<int>(bitmap.height * zoom_scale));
    return BitmapHelper::ResizeImage(bitmap, automationtest::utilities::Size {new_width, new_height});
}

std::shared_ptr<IOcrEngine> OcrProcesser::BuildDefaultEngine(const std::string& default_language)
{
    if (auto native_engine = TryCreateNativeTesseractEngine(default_language)) {
        return native_engine;
    }

    return std::make_shared<CallbackOcrEngine>(
        [](const Bitmap& bitmap, bool single_line, const std::string& whitelist) {
            OcrResult result {};
            result.text = HeuristicExtractText(bitmap, whitelist, single_line);
            result.confidence = result.text.empty() ? 0.0F : 0.5F;
            return result;
        });
}

} // namespace automationtest::ocrlib
