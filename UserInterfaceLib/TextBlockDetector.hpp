#ifndef AUTOMATIOTEST_USERINTERFACELIB_TEXTBLOCKDETECTOR_HPP
#define AUTOMATIOTEST_USERINTERFACELIB_TEXTBLOCKDETECTOR_HPP

#include "../Utilities/Common.hpp"
#include "../Utilities/Types/EnumRelativeLocation.hpp"

#include <regex>

namespace automationtest::userinterfacelib {

class TextBlockDetector {
public:
    explicit TextBlockDetector(const Bitmap& bitmap);
    TextBlockDetector(const std::vector<std::byte>& gray_values, int stride, int width, int height);

    [[nodiscard]] std::vector<std::pair<int, int>> GetLinesOfBitmapUsingBackgroundColor(int min_letter_height = 5) const;
    [[nodiscard]] std::vector<std::pair<int, int>> GetLinesOfBitmap(int start_col = 2, int end_col = -2) const;
    [[nodiscard]] int TryToFindIconOnTheLeft(std::optional<int> start_row_input = std::nullopt, std::optional<int> end_row_input = std::nullopt) const;

    static std::vector<Rectangle> FindTextBlocks(const Bitmap& bitmap, int min_size = 5);
    static std::vector<TextMatch> GetTextBlockAndText(const Bitmap& bitmap, const TextProvider& provider = {});
    static std::optional<TextMatch> GetTextBlockAndTextLined(const Bitmap& bitmap, const std::regex& regex, const TextProvider& provider = {});
    static std::vector<TextMatch> FindTextRandom(const Bitmap& bitmap, const std::regex& regex, const TextProvider& provider = {});
    static std::optional<Point> FindOneTextRandom(const Bitmap& bitmap, const Rectangle& popup_rectangle, const std::regex& regex, const TextProvider& provider = {});
    static std::optional<Rectangle> FindOneTextReturnRectangle(const Bitmap& bitmap, const std::regex& regex, const TextProvider& provider = {});
    static std::optional<Bitmap> ClickOnTextRandom(const Bitmap& bitmap, const Rectangle& popup_rectangle, const std::regex& regex, const ClickCallback& mouse_click, const TextProvider& provider = {}, int default_wait_ms = 1000);
    static std::optional<Bitmap> ClickOnTextWithShift(const Bitmap& bitmap, const Rectangle& popup_rectangle, double shift_in_height, const std::regex& regex, const ClickCallback& mouse_click, const TextProvider& provider = {}, int default_wait_ms = 1000);
    static std::optional<Bitmap> ClickOnTextNearText(const Bitmap& bitmap, const Rectangle& popup_rectangle, automationtest::utilities::types::EnumRelativeLocation relative_location, const std::regex& regex_for_text, const std::regex& regex_for_click, const ClickCallback& mouse_click, const TextProvider& provider = {}, int wait_ms = 1000);
    static std::optional<Rectangle> FindTextNearRectangle(const Bitmap& bitmap, const std::vector<Rectangle>& text_blocks, const Rectangle& rect, automationtest::utilities::types::EnumRelativeLocation location, const std::regex& regex_for_click, const TextProvider& provider = {});

private:
    Bitmap bitmap_ {};
};

} // namespace automationtest::userinterfacelib

#endif // AUTOMATIOTEST_USERINTERFACELIB_TEXTBLOCKDETECTOR_HPP
