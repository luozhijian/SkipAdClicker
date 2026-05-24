#ifndef AUTOMATIOTEST_USERINTERFACELIB_SCANBITMAPFORCONTROL_HPP
#define AUTOMATIOTEST_USERINTERFACELIB_SCANBITMAPFORCONTROL_HPP

#include "../Utilities/Common.hpp"
#include "Controls/UiBitmap.hpp"
#include "../Utilities/Settings/SettingRectangleDetection.hpp"
#include "../Utilities/RectanglePair.hpp"
#include "../Utilities/RectangleToRectangleList.hpp"
#include "../Utilities/TriangleWithDescription.hpp"

#include <chrono>
#include <optional>
#include <regex>
#include <string>

namespace automationtest::userinterfacelib {

class ScanBitmapForControl {
public:
    using ScreenshotProvider = std::function<std::optional<Bitmap>()>;
    using InputCallback = std::function<std::optional<Bitmap>(const Point&, const std::string&, int)>;

    ScanBitmapForControl(ClickCallback mouse_click = {},
        ClickCallback mouse_right_click = {},
        HoverCallback mouse_hover = {},
        InputCallback input_text = {},
        ScreenshotProvider screenshot_provider = {},
        TextProvider text_provider = {});

    static void Hello();
    static UiBitmap Parse(const Bitmap& bitmap);
    static void LogToWindow(const std::string& message);

    [[nodiscard]] std::vector<Rectangle> FindFillRectangleWithText(const Bitmap& image) const;
    [[nodiscard]] Rectangle FindRectangleWithText(const Bitmap& image, const std::regex& regex, const automationtest::utilities::settings::SettingRectangleDetection& setting = {}) const;
    [[nodiscard]] Rectangle FindRectangleWithTextFullRectangle(const Bitmap& image, const std::regex& regex, const std::string& target_string = {}, int threshold = 100) const;
    [[nodiscard]] static Bitmap GetBitmapByPart(const Bitmap& bitmap, const Rectangle& rectangle);

    std::optional<Bitmap> FindRectangleWithTextAndInput(const Bitmap& image, const automationtest::utilities::settings::SettingRectangleDetection& setting, const std::regex& regex, const std::string& input_text, double second_wait = 0.2) const;
    std::optional<Bitmap> FindRectangleWithTextAndInput(const Point& point, const std::string& input_text, int ms_wait = 200) const;
    std::optional<Bitmap> ClickNearPoint(const Point& point, int x_shift, int y_shift, int ms_wait = 200) const;
    std::optional<Bitmap> FindRectangleWithTextAndClick(const Bitmap& image, const std::regex& regex, const std::string& target_string, int threshold, int ms_wait = 200) const;

    std::optional<Bitmap> ClickOnText(const Bitmap& image, const std::regex& regex, int default_time_ms = 1000, const std::optional<Rectangle>& range_rectangle = std::nullopt) const;
    std::optional<Bitmap> ClickOnTextWithShift(const Bitmap& image, const std::regex& regex, double shift_in_height, int default_time_ms = 1000, const std::optional<Rectangle>& range_rectangle = std::nullopt) const;
    bool FindOneText(const Bitmap& image, const std::regex& regex, const std::optional<Rectangle>& range_rectangle = std::nullopt) const;
    std::optional<Bitmap> MouseHoverOnPoint(const Point& location, int delay_ms_before_screenshot = 1000) const;
    std::optional<Bitmap> MouseHoverOnTextRandom(const Bitmap& image, const std::regex& regex, int delay_before_screenshot = 1000) const;
    std::optional<Bitmap> MouseHoverOnTextPartOfScreen(const Bitmap& image, const Rectangle& part_location, const std::regex& regex, int delay_before_screenshot = 1000) const;

    [[nodiscard]] automationtest::utilities::TriangleWithDescription FindSmallTriangleInBox(const Bitmap& image, const Rectangle& location) const;
    [[nodiscard]] std::vector<automationtest::utilities::RectangleToRectangleList> FindTextLinkToRectangles(const Bitmap& bitmap, const std::vector<Rectangle>& texts, const std::vector<Rectangle>& rectangles, const std::string& text, int size_filter_for_rectangle, bool above) const;
    [[nodiscard]] std::vector<automationtest::utilities::RectanglePair> FindTextLinkToRectangle(const Bitmap& image) const;
    [[nodiscard]] automationtest::utilities::RectanglePair FindFirstRectangleLinkToText(const std::vector<automationtest::utilities::RectanglePair>& rectangle_pairs, const std::regex& regex) const;

    std::optional<Bitmap> ClickOnPoint(const Point& point, double default_time_seconds = 0.1) const;
    std::optional<Bitmap> RightClickOnPoint(const Point& point, double default_time_seconds = 1.0) const;
    static void SleepSeconds(double seconds);
    std::optional<Bitmap> TakeScreenShot() const;

private:
    [[nodiscard]] std::string ReadText(const Bitmap& bitmap, const std::string& fallback = {}) const;
    [[nodiscard]] std::optional<Bitmap> ScreenshotAfterDelay(int delay_ms) const;

    ClickCallback mouse_click_ {};
    ClickCallback mouse_right_click_ {};
    HoverCallback mouse_hover_ {};
    InputCallback input_text_ {};
    ScreenshotProvider screenshot_provider_ {};
    TextProvider text_provider_ {};
};

} // namespace automationtest::userinterfacelib

#endif // AUTOMATIOTEST_USERINTERFACELIB_SCANBITMAPFORCONTROL_HPP
