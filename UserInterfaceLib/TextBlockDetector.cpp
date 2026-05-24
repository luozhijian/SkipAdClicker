#include "TextBlockDetector.hpp"

#include "../OpencvLib/OpenCvLib.hpp"
#include "../Utilities/BitmapHelper.hpp"
#include "../Utilities/DrawingLib.hpp"
#include "../Utilities/RectangleHelper.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <queue>
#include <unordered_map>

namespace automationtest::userinterfacelib {

using automationtest::utilities::BitmapHelper;
using automationtest::utilities::DrawingLib;
using automationtest::utilities::RectangleHelper;
using automationtest::utilities::types::EnumRelativeLocation;

namespace {

std::uint8_t PixelAt(const Bitmap& bitmap, int row, int col)
{
    return static_cast<std::uint8_t>(bitmap.pixels[static_cast<std::size_t>(row) * static_cast<std::size_t>(bitmap.stride) + static_cast<std::size_t>(col)]);
}

std::optional<Bitmap> ClickAtRectangle(const Rectangle& rectangle, const Rectangle& popup_rectangle, const ClickCallback& callback, int wait_ms)
{
    if (!callback) {
        return std::nullopt;
    }
    const auto point = Point {popup_rectangle.x + rectangle.x + rectangle.width / 2, popup_rectangle.y + rectangle.y + rectangle.height / 2};
    return callback(point, wait_ms);
}

std::vector<std::pair<int, int>> GroupBooleanRuns(const std::vector<bool>& values)
{
    std::vector<std::pair<int, int>> result {};
    int index = 0;
    while (index < static_cast<int>(values.size())) {
        while (index < static_cast<int>(values.size()) && values[static_cast<std::size_t>(index)]) {
            ++index;
        }
        if (index >= static_cast<int>(values.size())) {
            break;
        }
        const auto start = std::max(0, index - 1);
        while (index < static_cast<int>(values.size()) && !values[static_cast<std::size_t>(index)]) {
            ++index;
        }
        result.emplace_back(start, std::min(index + 1, static_cast<int>(values.size())));
    }
    return result;
}

bool MatchesRelativeLocation(const Rectangle& source, const Rectangle& candidate, EnumRelativeLocation location)
{
    switch (location) {
    case EnumRelativeLocation::Above:
        return candidate.y + candidate.height <= source.y
            && candidate.x <= source.x + source.width
            && candidate.x + candidate.width >= source.x;
    case EnumRelativeLocation::Below:
        return candidate.y >= source.y + source.height
            && candidate.x <= source.x + source.width
            && candidate.x + candidate.width >= source.x;
    case EnumRelativeLocation::Left:
        return candidate.x + candidate.width <= source.x
            && candidate.y <= source.y + source.height
            && candidate.y + candidate.height >= source.y;
    case EnumRelativeLocation::Right:
        return candidate.x >= source.x + source.width
            && candidate.y <= source.y + source.height
            && candidate.y + candidate.height >= source.y;
    case EnumRelativeLocation::SameRowLeftClose:
        return std::abs(candidate.y - source.y) <= std::max(source.height, candidate.height)
            && candidate.x < source.x
            && std::abs(candidate.x - source.x) <= std::max(source.height, candidate.height) * 3;
    case EnumRelativeLocation::SameRowRightClose:
        return std::abs(candidate.y - source.y) <= std::max(source.height, candidate.height)
            && candidate.x > source.x
            && std::abs(candidate.x - source.x) <= std::max(source.height, candidate.height) * 3;
    default:
        return false;
    }
}

} // namespace

TextBlockDetector::TextBlockDetector(const Bitmap& bitmap)
    : bitmap_(BitmapHelper::ConvertToGrayscale(bitmap))
{
}

TextBlockDetector::TextBlockDetector(const std::vector<std::byte>& gray_values, int stride, int width, int height)
{
    bitmap_.width = width;
    bitmap_.height = height;
    bitmap_.stride = stride;
    bitmap_.channels = 1;
    bitmap_.pixels = gray_values;
}

std::vector<std::pair<int, int>> TextBlockDetector::GetLinesOfBitmapUsingBackgroundColor(int min_letter_height) const
{
    if (bitmap_.pixels.empty() || bitmap_.height <= 0 || bitmap_.width <= 0) {
        return {};
    }

    std::unordered_map<std::uint8_t, int> histogram {};
    for (const auto& value : bitmap_.pixels) {
        ++histogram[static_cast<std::uint8_t>(value)];
    }

    const auto background = std::max_element(histogram.begin(), histogram.end(), [](const auto& left, const auto& right) {
        return left.second < right.second;
    })->first;

    std::vector<bool> background_lines(static_cast<std::size_t>(bitmap_.height), true);
    for (int row = 0; row < bitmap_.height; ++row) {
        for (int col = 0; col < bitmap_.width; ++col) {
            if (PixelAt(bitmap_, row, col) != background) {
                background_lines[static_cast<std::size_t>(row)] = false;
                break;
            }
        }
    }

    auto result = GroupBooleanRuns(background_lines);
    result.erase(std::remove_if(result.begin(), result.end(), [min_letter_height](const auto& segment) {
        return (segment.second - segment.first) < min_letter_height;
    }), result.end());
    return result;
}

std::vector<std::pair<int, int>> TextBlockDetector::GetLinesOfBitmap(int start_col, int end_col) const
{
    if (bitmap_.pixels.empty() || bitmap_.height <= 0 || bitmap_.width <= 0) {
        return {};
    }

    if (end_col <= 0) {
        end_col = std::max(bitmap_.width + end_col, 0);
    }
    start_col = std::clamp(start_col, 0, std::max(0, bitmap_.width - 1));
    end_col = std::clamp(end_col, start_col + 1, bitmap_.width);

    std::vector<int> sums(static_cast<std::size_t>(bitmap_.height), 0);
    int max_sum = std::numeric_limits<int>::min();
    int min_sum = std::numeric_limits<int>::max();
    int max_sum_count = 0;
    int min_sum_count = 0;

    for (int row = 0; row < bitmap_.height; ++row) {
        int sum = 0;
        for (int col = start_col; col < end_col; ++col) {
            sum += PixelAt(bitmap_, row, col);
        }
        sums[static_cast<std::size_t>(row)] = sum;
        if (sum == min_sum) {
            ++min_sum_count;
        } else if (sum < min_sum) {
            min_sum = sum;
            min_sum_count = 1;
        }

        if (sum == max_sum) {
            ++max_sum_count;
        } else if (sum > max_sum) {
            max_sum = sum;
            max_sum_count = 1;
        }
    }

    const auto white_line_sum = max_sum_count > min_sum_count ? max_sum : min_sum;
    std::vector<bool> white_lines(static_cast<std::size_t>(bitmap_.height), false);
    for (int row = 0; row < bitmap_.height; ++row) {
        white_lines[static_cast<std::size_t>(row)] = sums[static_cast<std::size_t>(row)] == white_line_sum;
    }

    return GroupBooleanRuns(white_lines);
}

int TextBlockDetector::TryToFindIconOnTheLeft(std::optional<int> start_row_input, std::optional<int> end_row_input) const
{
    if (bitmap_.pixels.empty() || bitmap_.height <= 0 || bitmap_.width <= 0) {
        return -1;
    }

    const auto start_row = start_row_input.value_or(bitmap_.height / 3);
    const auto end_row = std::clamp(end_row_input.value_or(bitmap_.height * 2 / 3), start_row + 1, bitmap_.height);

    std::vector<int> sums(static_cast<std::size_t>(bitmap_.width), 0);
    int max_sum = std::numeric_limits<int>::min();
    int min_sum = std::numeric_limits<int>::max();
    int max_sum_count = 0;
    int min_sum_count = 0;

    for (int col = 0; col < bitmap_.width; ++col) {
        int sum = 0;
        for (int row = start_row; row < end_row; ++row) {
            sum += PixelAt(bitmap_, row, col);
        }
        sums[static_cast<std::size_t>(col)] = sum;
        if (sum == min_sum) {
            ++min_sum_count;
        } else if (sum < min_sum) {
            min_sum = sum;
            min_sum_count = 1;
        }
        if (sum == max_sum) {
            ++max_sum_count;
        } else if (sum > max_sum) {
            max_sum = sum;
            max_sum_count = 1;
        }
    }

    const auto white_line_sum = max_sum_count > min_sum_count ? max_sum : min_sum;
    std::vector<bool> white_lines(static_cast<std::size_t>(bitmap_.width), false);
    for (int col = 0; col < bitmap_.width; ++col) {
        white_lines[static_cast<std::size_t>(col)] = sums[static_cast<std::size_t>(col)] == white_line_sum;
    }

    const auto segments = GroupBooleanRuns(white_lines);
    if (segments.size() >= 2) {
        return (segments[0].second + segments[1].first) / 2;
    }
    return -1;
}

std::vector<Rectangle> TextBlockDetector::FindTextBlocks(const Bitmap& bitmap, int min_size)
{
    auto gray = BitmapHelper::ConvertToGrayscale(bitmap);
    std::vector<Rectangle> result {};
    std::vector<std::uint8_t> visited(static_cast<std::size_t>(gray.width) * static_cast<std::size_t>(gray.height), 0);

    auto index_of = [&gray](int row, int col) {
        return static_cast<std::size_t>(row) * static_cast<std::size_t>(gray.width) + static_cast<std::size_t>(col);
    };

    for (int row = 0; row < gray.height; ++row) {
        for (int col = 0; col < gray.width; ++col) {
            if (visited[index_of(row, col)] != 0 || PixelAt(gray, row, col) > 220) {
                continue;
            }

            std::queue<Point> queue {};
            queue.push(Point {col, row});
            visited[index_of(row, col)] = 1;
            int min_x = col;
            int min_y = row;
            int max_x = col;
            int max_y = row;
            int count = 0;

            while (!queue.empty()) {
                const auto point = queue.front();
                queue.pop();
                ++count;
                min_x = std::min(min_x, point.x);
                min_y = std::min(min_y, point.y);
                max_x = std::max(max_x, point.x);
                max_y = std::max(max_y, point.y);

                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        if (dx == 0 && dy == 0) {
                            continue;
                        }
                        const auto next_x = point.x + dx;
                        const auto next_y = point.y + dy;
                        if (next_x < 0 || next_y < 0 || next_x >= gray.width || next_y >= gray.height) {
                            continue;
                        }
                        const auto idx = index_of(next_y, next_x);
                        if (visited[idx] != 0 || PixelAt(gray, next_y, next_x) > 220) {
                            continue;
                        }
                        visited[idx] = 1;
                        queue.push(Point {next_x, next_y});
                    }
                }
            }

            if (count >= min_size) {
                result.push_back(RectangleHelper::FromXYXY(min_x, min_y, max_x + 1, max_y + 1));
            }
        }
    }

    return result;
}

std::vector<TextMatch> TextBlockDetector::GetTextBlockAndText(const Bitmap& bitmap, const TextProvider& provider)
{
    std::vector<TextMatch> result {};
    for (const auto& rectangle : FindTextBlocks(bitmap)) {
        result.push_back(TextMatch {rectangle, provider ? provider(BitmapHelper::GetBitmap(bitmap, rectangle)) : std::string {}});
    }
    return result;
}

std::optional<TextMatch> TextBlockDetector::GetTextBlockAndTextLined(const Bitmap& bitmap, const std::regex& regex, const TextProvider& provider)
{
    for (const auto& match : GetTextBlockAndText(bitmap, provider)) {
        if (std::regex_search(match.text, regex)) {
            return match;
        }
    }
    return std::nullopt;
}

std::vector<TextMatch> TextBlockDetector::FindTextRandom(const Bitmap& bitmap, const std::regex& regex, const TextProvider& provider)
{
    std::vector<TextMatch> result {};
    for (const auto& match : GetTextBlockAndText(bitmap, provider)) {
        auto normalized = match.text;
        normalized.erase(std::remove_if(normalized.begin(), normalized.end(), [](unsigned char value) {
            return value == '\r' || value == '\n';
        }), normalized.end());
        if (std::regex_search(normalized, regex)) {
            result.push_back(match);
        }
    }
    return result;
}

std::optional<Point> TextBlockDetector::FindOneTextRandom(const Bitmap& bitmap, const Rectangle& popup_rectangle, const std::regex& regex, const TextProvider& provider)
{
    const auto search_bitmap = popup_rectangle.width > 0 && popup_rectangle.height > 0
        ? BitmapHelper::GetBitmap(bitmap, popup_rectangle)
        : bitmap;
    const auto rectangle = FindOneTextReturnRectangle(search_bitmap, regex, provider);
    if (!rectangle.has_value()) {
        return std::nullopt;
    }

    return Point {
        popup_rectangle.x + rectangle->x + rectangle->width / 2,
        popup_rectangle.y + rectangle->y + rectangle->height / 2
    };
}

std::optional<Rectangle> TextBlockDetector::FindOneTextReturnRectangle(const Bitmap& bitmap, const std::regex& regex, const TextProvider& provider)
{
    const auto matches = FindTextRandom(bitmap, regex, provider);
    if (matches.empty()) {
        return std::nullopt;
    }
    return matches.front().rectangle;
}

std::optional<Bitmap> TextBlockDetector::ClickOnTextRandom(const Bitmap& bitmap, const Rectangle& popup_rectangle, const std::regex& regex, const ClickCallback& mouse_click, const TextProvider& provider, int default_wait_ms)
{
    const auto point = FindOneTextRandom(bitmap, popup_rectangle, regex, provider);
    if (!point.has_value() || !mouse_click) {
        return std::nullopt;
    }
    return mouse_click(point.value(), default_wait_ms);
}

std::optional<Bitmap> TextBlockDetector::ClickOnTextWithShift(const Bitmap& bitmap, const Rectangle& popup_rectangle, double shift_in_height, const std::regex& regex, const ClickCallback& mouse_click, const TextProvider& provider, int default_wait_ms)
{
    const auto search_bitmap = popup_rectangle.width > 0 && popup_rectangle.height > 0
        ? BitmapHelper::GetBitmap(bitmap, popup_rectangle)
        : bitmap;
    const auto rectangle = FindOneTextReturnRectangle(search_bitmap, regex, provider);
    if (!rectangle.has_value() || !mouse_click) {
        return std::nullopt;
    }

    const auto point = Point {
        popup_rectangle.x + rectangle->x + static_cast<int>(std::lround(rectangle->height * shift_in_height)),
        popup_rectangle.y + rectangle->y + rectangle->height / 2
    };
    return mouse_click(point, default_wait_ms);
}

std::optional<Bitmap> TextBlockDetector::ClickOnTextNearText(const Bitmap& bitmap, const Rectangle& popup_rectangle, EnumRelativeLocation relative_location, const std::regex& regex_for_text, const std::regex& regex_for_click, const ClickCallback& mouse_click, const TextProvider& provider, int wait_ms)
{
    const auto text_matches = FindTextRandom(bitmap, regex_for_text, provider);
    const auto blocks = GetTextBlockAndText(bitmap, provider);

    for (const auto& text_match : text_matches) {
        std::vector<Rectangle> rectangles {};
        rectangles.reserve(blocks.size());
        for (const auto& block : blocks) {
            rectangles.push_back(block.rectangle);
        }

        const auto candidate = FindTextNearRectangle(bitmap, rectangles, text_match.rectangle, relative_location, regex_for_click, provider);
        if (candidate.has_value()) {
            return ClickAtRectangle(candidate.value(), popup_rectangle, mouse_click, wait_ms);
        }
    }
    return std::nullopt;
}

std::optional<Rectangle> TextBlockDetector::FindTextNearRectangle(const Bitmap& bitmap, const std::vector<Rectangle>& text_blocks, const Rectangle& rect, EnumRelativeLocation location, const std::regex& regex_for_click, const TextProvider& provider)
{
    std::optional<Rectangle> best {};
    int best_score = std::numeric_limits<int>::max();
    for (const auto& candidate : text_blocks) {
        if (!MatchesRelativeLocation(rect, candidate, location)) {
            continue;
        }

        const auto text = provider ? provider(BitmapHelper::GetBitmap(bitmap, candidate)) : std::string {};
        if (!(text.empty() || std::regex_search(text, regex_for_click))) {
            continue;
        }

        const auto score = std::abs(candidate.x - rect.x) + std::abs(candidate.y - rect.y);
        if (!best.has_value() || score < best_score) {
            best = candidate;
            best_score = score;
        }
    }

    return best;
}

} // namespace automationtest::userinterfacelib
