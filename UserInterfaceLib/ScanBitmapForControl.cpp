#include "ScanBitmapForControl.hpp"

#include "TextBlockDetector.hpp"
#include "../OpencvLib/OpenCvLib.hpp"
#include "../Utilities/BitmapHelper.hpp"
#include "../Utilities/RectangleHelper.hpp"

#include <chrono>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <thread>

namespace automationtest::userinterfacelib {

using automationtest::opencvlib::OpenCvLib;
using automationtest::utilities::settings::SettingRectangleDetection;
using automationtest::utilities::BitmapHelper;
using automationtest::utilities::RectangleHelper;
using automationtest::utilities::RectanglePair;
using automationtest::utilities::RectangleToRectangleList;
using automationtest::utilities::TriangleWithDescription;
using automationtest::utilities::types::EnumRelativeLocation;

namespace {

int Milliseconds(double seconds)
{
    return static_cast<int>(seconds * 1000.0);
}

std::string RegexMessage(const char* prefix)
{
    return std::string(prefix);
}

} // namespace

ScanBitmapForControl::ScanBitmapForControl(ClickCallback mouse_click,
    ClickCallback mouse_right_click,
    HoverCallback mouse_hover,
    InputCallback input_text,
    ScreenshotProvider screenshot_provider,
    TextProvider text_provider)
    : mouse_click_(std::move(mouse_click))
    , mouse_right_click_(std::move(mouse_right_click))
    , mouse_hover_(std::move(mouse_hover))
    , input_text_(std::move(input_text))
    , screenshot_provider_(std::move(screenshot_provider))
    , text_provider_(std::move(text_provider))
{
}

void ScanBitmapForControl::Hello()
{
}

UiBitmap ScanBitmapForControl::Parse(const Bitmap& bitmap)
{
    UiBitmap ui_bitmap(bitmap);
    ui_bitmap.ParseBitmap();
    return ui_bitmap;
}

void ScanBitmapForControl::LogToWindow(const std::string& message)
{
    std::cout << message << '\n';
}

std::vector<Rectangle> ScanBitmapForControl::FindFillRectangleWithText(const Bitmap& image) const
{
    return OpenCvLib::FindHorizonAndVerticalRectangles(OpenCvLib::ToGrayMat(image));
}

Rectangle ScanBitmapForControl::FindRectangleWithText(const Bitmap& image, const std::regex& regex, const SettingRectangleDetection& setting) const
{
    const auto rectangles = OpenCvLib::FindHorizonAndVerticalRectangles(OpenCvLib::ToGrayMat(image), &setting);
    for (const auto& rect : rectangles) {
        if (rect.IsEmpty()) {
            continue;
        }

        const auto text = ReadText(BitmapHelper::GetBitmap(image, rect));
        if (std::regex_search(text, regex)) {
            return rect;
        }
    }

    throw std::runtime_error(RegexMessage("Cannot find rectangle with text"));
}

Rectangle ScanBitmapForControl::FindRectangleWithTextFullRectangle(const Bitmap& image, const std::regex& regex, const std::string& target_string, int threshold) const
{
    const auto rectangles = OpenCvLib::FindHorizonAndVerticalRectangles(OpenCvLib::ToGrayMat(image));
    for (const auto& rect : rectangles) {
        if (rect.width < threshold || rect.IsEmpty()) {
            continue;
        }

        const auto text = ReadText(BitmapHelper::GetBitmap(image, rect), target_string);
        if (std::regex_search(text, regex)) {
            return rect;
        }
    }

    throw std::runtime_error(RegexMessage("Cannot find rectangle with text"));
}

Bitmap ScanBitmapForControl::GetBitmapByPart(const Bitmap& bitmap, const Rectangle& rectangle)
{
    return BitmapHelper::GetBitmap(bitmap, rectangle);
}

std::optional<Bitmap> ScanBitmapForControl::FindRectangleWithTextAndInput(const Bitmap& image, const SettingRectangleDetection& setting, const std::regex& regex, const std::string& input_text, double second_wait) const
{
    const auto rectangle = FindRectangleWithText(image, regex, setting);
    return FindRectangleWithTextAndInput(RectangleHelper::Center(rectangle), input_text, Milliseconds(second_wait));
}

std::optional<Bitmap> ScanBitmapForControl::FindRectangleWithTextAndInput(const Point& point, const std::string& input_text, int ms_wait) const
{
    return input_text_ ? input_text_(point, input_text, ms_wait) : std::nullopt;
}

std::optional<Bitmap> ScanBitmapForControl::ClickNearPoint(const Point& point, int x_shift, int y_shift, int ms_wait) const
{
    return mouse_click_ ? mouse_click_(Point {point.x + x_shift, point.y + y_shift}, ms_wait) : std::nullopt;
}

std::optional<Bitmap> ScanBitmapForControl::FindRectangleWithTextAndClick(const Bitmap& image, const std::regex& regex, const std::string& target_string, int threshold, int ms_wait) const
{
    const auto rectangle = FindRectangleWithTextFullRectangle(image, regex, target_string, threshold);
    return ClickOnPoint(RectangleHelper::Center(rectangle), ms_wait / 1000.0);
}

std::optional<Bitmap> ScanBitmapForControl::ClickOnText(const Bitmap& image, const std::regex& regex, int default_time_ms, const std::optional<Rectangle>& range_rectangle) const
{
    return TextBlockDetector::ClickOnTextRandom(image, range_rectangle.value_or(Rectangle {}), regex, mouse_click_, text_provider_, default_time_ms);
}

std::optional<Bitmap> ScanBitmapForControl::ClickOnTextWithShift(const Bitmap& image, const std::regex& regex, double shift_in_height, int default_time_ms, const std::optional<Rectangle>& range_rectangle) const
{
    return TextBlockDetector::ClickOnTextWithShift(image, range_rectangle.value_or(Rectangle {}), shift_in_height, regex, mouse_click_, text_provider_, default_time_ms);
}

bool ScanBitmapForControl::FindOneText(const Bitmap& image, const std::regex& regex, const std::optional<Rectangle>& range_rectangle) const
{
    const auto search_bitmap = range_rectangle.has_value() ? BitmapHelper::GetBitmap(image, range_rectangle.value()) : image;
    return TextBlockDetector::FindOneTextReturnRectangle(search_bitmap, regex, text_provider_).has_value();
}

std::optional<Bitmap> ScanBitmapForControl::MouseHoverOnPoint(const Point& location, int delay_ms_before_screenshot) const
{
    if (mouse_hover_) {
        if (auto result = mouse_hover_(location, delay_ms_before_screenshot); result.has_value()) {
            return result;
        }
    }
    return ScreenshotAfterDelay(delay_ms_before_screenshot);
}

std::optional<Bitmap> ScanBitmapForControl::MouseHoverOnTextRandom(const Bitmap& image, const std::regex& regex, int delay_before_screenshot) const
{
    const auto point = TextBlockDetector::FindOneTextRandom(image, Rectangle {}, regex, text_provider_);
    return point.has_value() ? MouseHoverOnPoint(point.value(), delay_before_screenshot) : std::nullopt;
}

std::optional<Bitmap> ScanBitmapForControl::MouseHoverOnTextPartOfScreen(const Bitmap& image, const Rectangle& part_location, const std::regex& regex, int delay_before_screenshot) const
{
    const auto point = TextBlockDetector::FindOneTextRandom(image, part_location, regex, text_provider_);
    return point.has_value() ? MouseHoverOnPoint(point.value(), delay_before_screenshot) : std::nullopt;
}

TriangleWithDescription ScanBitmapForControl::FindSmallTriangleInBox(const Bitmap& image, const Rectangle& location) const
{
    const auto cropped = BitmapHelper::GetBitmap(image, location);
    const auto triangles = OpenCvLib::FindSmallTriangles(cropped);
    if (triangles.empty()) {
        throw std::runtime_error("Cannot find drop down");
    }

    return triangles.front().CreateAShift(Size {location.x, location.y});
}

std::vector<RectangleToRectangleList> ScanBitmapForControl::FindTextLinkToRectangles(const Bitmap& bitmap, const std::vector<Rectangle>& texts, const std::vector<Rectangle>& rectangles, const std::string& text, int size_filter_for_rectangle, bool above) const
{
    std::vector<RectangleToRectangleList> results {};
    for (const auto& text_rect : texts) {
        const auto detected_text = ReadText(BitmapHelper::GetBitmap(bitmap, text_rect), text.empty() ? std::string {} : text + " ");
        if (detected_text.find(text) == std::string::npos) {
            continue;
        }

        RectangleToRectangleList item {};
        item.rect = text_rect;
        item.text = detected_text;
        for (const auto& rectangle : rectangles) {
            if (RectangleHelper::Area(rectangle) < size_filter_for_rectangle) {
                continue;
            }
            const auto relative_location = above ? EnumRelativeLocation::Below : EnumRelativeLocation::Above;
            if (RectangleHelper::CheckRelativeLocation(text_rect, rectangle, relative_location)) {
                item.AddRectangle(rectangle);
            }
        }
        results.push_back(std::move(item));
    }
    return results;
}

std::vector<RectanglePair> ScanBitmapForControl::FindTextLinkToRectangle(const Bitmap& image) const
{
    const auto text_blocks = TextBlockDetector::FindTextBlocks(image, 5);
    const auto rectangles = OpenCvLib::FindHorizonAndVerticalRectangles(OpenCvLib::ToGrayMat(image));
    std::vector<RectanglePair> result {};

    for (const auto& rectangle : rectangles) {
        std::optional<Rectangle> best_text {};
        int best_distance = std::numeric_limits<int>::max();
        for (const auto& text_block : text_blocks) {
            if (!RectangleHelper::CheckRelativeLocation(rectangle, text_block, EnumRelativeLocation::Above, std::max(20, rectangle.height), 4)) {
                continue;
            }
            const int distance = rectangle.Top() - text_block.Bottom();
            if (distance >= 0 && distance < best_distance) {
                best_text = text_block;
                best_distance = distance;
            }
        }

        if (best_text.has_value()) {
            RectanglePair pair {rectangle, best_text.value()};
            pair.text = ReadText(BitmapHelper::GetBitmap(image, best_text.value()));
            result.push_back(std::move(pair));
        }
    }

    return result;
}

RectanglePair ScanBitmapForControl::FindFirstRectangleLinkToText(const std::vector<RectanglePair>& rectangle_pairs, const std::regex& regex) const
{
    std::optional<RectanglePair> result {};
    int min_x_y = std::numeric_limits<int>::max();
    for (const auto& pair : rectangle_pairs) {
        if (!std::regex_search(pair.text, regex)) {
            continue;
        }
        const int score = pair.rect1.x + pair.rect1.y;
        if (!result.has_value() || score < min_x_y) {
            result = pair;
            min_x_y = score;
        }
    }

    if (!result.has_value()) {
        throw std::runtime_error("Cannot find rectangle with text");
    }
    return result.value();
}

std::optional<Bitmap> ScanBitmapForControl::ClickOnPoint(const Point& point, double default_time_seconds) const
{
    return mouse_click_ ? mouse_click_(point, Milliseconds(default_time_seconds)) : std::nullopt;
}

std::optional<Bitmap> ScanBitmapForControl::RightClickOnPoint(const Point& point, double default_time_seconds) const
{
    return mouse_right_click_ ? mouse_right_click_(point, Milliseconds(default_time_seconds)) : std::nullopt;
}

void ScanBitmapForControl::SleepSeconds(double seconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(Milliseconds(seconds)));
}

std::optional<Bitmap> ScanBitmapForControl::TakeScreenShot() const
{
    return screenshot_provider_ ? screenshot_provider_() : std::nullopt;
}


std::string ScanBitmapForControl::ReadText(const Bitmap& bitmap, const std::string& fallback) const
{
    if (text_provider_) {
        return text_provider_(bitmap);
    }
    return fallback;
}

std::optional<Bitmap> ScanBitmapForControl::ScreenshotAfterDelay(int delay_ms) const
{
    if (delay_ms > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    }
    return TakeScreenShot();
}

} // namespace automationtest::userinterfacelib
