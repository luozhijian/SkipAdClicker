#include "SkipAdDetector.hpp"

#include "../OpencvLib/MatExtension.hpp"
#include "../OpencvLib/OpenCvLib.hpp"
#include "../OpencvLib/StripeDetector.hpp"
#include "../Utilities/BitmapHelper.hpp"
#include "../Utilities/Exceptions/TestException.hpp"
#include "../Utilities/InputDevice/MouseKeyboardLib.hpp"
#include "../Utilities/PointHelper.hpp"
#include "../Utilities/RectangleHelper.hpp"
#include "../Utilities/Logger.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <optional>
#include <vector>

#include <opencv2/imgproc.hpp>

namespace automationtest::userinterfacelib {

using automationtest::opencvlib::MatExtension;
using automationtest::opencvlib::OpenCvLib;
using automationtest::opencvlib::StripeDetector;
using automationtest::utilities::BitmapHelper;
using automationtest::utilities::Logger;
using automationtest::utilities::MouseKeyboardLib;
using automationtest::utilities::PointHelper;
using automationtest::utilities::RectangleHelper;
using automationtest::utilities::TriangleWithDescription;
using automationtest::utilities::exceptions::TestException;
using automationtest::utilities::settings::SettingCanny;
using automationtest::utilities::settings::SettingLineDetection;

namespace {

cv::Mat CropMat(const cv::Mat& input, const Rectangle& rect)
{
    if (input.empty() || rect.IsEmpty()) {
        return {};
    }

    const int left = std::max(0, rect.Left());
    const int top = std::max(0, rect.Top());
    const int right = std::min(input.cols, rect.Right());
    const int bottom = std::min(input.rows, rect.Bottom());
    if (left >= right || top >= bottom) {
        return {};
    }

    return input(cv::Rect(left, top, right - left, bottom - top));
}

cv::Mat BitmapRegionToGrayMat(const Bitmap& bitmap, const Rectangle& region)
{
    if (bitmap.width <= 0 || bitmap.height <= 0 || bitmap.pixels.empty()) {
        return {};
    }

    const int channels = bitmap.channels <= 0 ? 1 : bitmap.channels;
    const int stride = bitmap.stride > 0 ? bitmap.stride : bitmap.width * channels;
    const int left = std::max(0, region.Left());
    const int top = std::max(0, region.Top());
    const int right = std::min(bitmap.width, region.Right());
    const int bottom = std::min(bitmap.height, region.Bottom());
    if (left >= right || top >= bottom) {
        return {};
    }
    const Rectangle bounded {left, top, right - left, bottom - top};

    const auto offset =
        static_cast<std::size_t>(bounded.y) * static_cast<std::size_t>(stride)
        + static_cast<std::size_t>(bounded.x) * static_cast<std::size_t>(channels);
    if (offset >= bitmap.pixels.size()) {
        return {};
    }

    const int type = CV_MAKETYPE(CV_8U, std::max(1, channels));
    cv::Mat view(bounded.height, bounded.width, type, const_cast<std::byte*>(bitmap.pixels.data() + offset), stride);
    if (channels == 1) {
        return view.clone();
    }

    cv::Mat gray {};
    if (channels == 4) {
        cv::cvtColor(view, gray, cv::COLOR_BGRA2GRAY);
    } else if (channels == 3) {
        cv::cvtColor(view, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = view.reshape(1).clone();
    }
    return gray;
}

Rectangle Intersect(const Rectangle& a, const Rectangle& b)
{
    const int left = std::max(a.Left(), b.Left());
    const int top = std::max(a.Top(), b.Top());
    const int right = std::min(a.Right(), b.Right());
    const int bottom = std::min(a.Bottom(), b.Bottom());
    if (left >= right || top >= bottom) {
        return {};
    }
    return Rectangle {left, top, right - left, bottom - top};
}

Rectangle ImageRectangle(const cv::Mat& image)
{
    return Rectangle {0, 0, image.cols, image.rows};
}

struct SkipTriangleParts {
    Point upper {};
    Point lower {};
    Point tip {};
};

std::optional<SkipTriangleParts> OrientAsRightPointingTriangle(const TriangleWithDescription& triangle)
{
    const auto points = triangle.AsArray();
    struct Candidate {
        int first {};
        int second {};
        int third {};
        int score {std::numeric_limits<int>::max()};
    };

    std::optional<Candidate> best {};
    for (int first = 0; first < 3; ++first) {
        for (int second = first + 1; second < 3; ++second) {
            const int third = 3 - first - second;
            const int x_delta = std::abs(points[first].x - points[second].x);
            const int tip_distance = points[third].x - std::max(points[first].x, points[second].x);
            if (tip_distance <= 0) {
                continue;
            }
            const int score = x_delta - tip_distance;
            if (!best.has_value() || score < best->score) {
                best = Candidate {first, second, third, score};
            }
        }
    }

    if (!best.has_value()) {
        return std::nullopt;
    }

    Point p1 = points[best->first];
    Point p2 = points[best->second];
    if (p1.y > p2.y) {
        std::swap(p1, p2);
    }
    return SkipTriangleParts {p1, p2, points[best->third]};
}

double RelativeDifference(int left, int right)
{
    return std::abs(left - right) * 1.0 / std::max(1, std::abs(right));
}

std::pair<int, float> GetMaxColorInRectangle(const cv::Mat& gray, const Rectangle& rect)
{
    const auto cropped = CropMat(gray, rect);
    if (cropped.empty()) {
        return {0, 0.0F};
    }

    std::vector<int> histogram(256, 0);
    int total = 0;
    for (int row = 0; row < cropped.rows; ++row) {
        for (int col = 0; col < cropped.cols; ++col) {
            ++histogram[MatExtension::GetByteValue(cropped, row, col)];
            ++total;
        }
    }
    if (total == 0) {
        return {0, 0.0F};
    }

    const auto max_iterator = std::max_element(histogram.begin(), histogram.end());
    const int color = static_cast<int>(std::distance(histogram.begin(), max_iterator));
    return {color, *max_iterator / static_cast<float>(total)};
}

Rectangle ExpandSkipAdToSmallRectangleWord(const cv::Mat& image, const TriangleWithDescription& triangle)
{
    const auto covered = triangle.GetCoveredRectangle();
    const int x = covered.Left() - static_cast<int>(covered.width * 4);
    const int y = covered.Top() - 2;
    if (x < 0 || y < 0) {
        return {};
    }

    const Rectangle candidate {x, y, covered.width * 4 - 2, covered.height + 4};
    return Intersect(ImageRectangle(image), candidate);
}

std::pair<Rectangle, Rectangle> ExpandSkipAdToUpperAndDownRectangle(const cv::Mat& image, const TriangleWithDescription& triangle)
{
    const auto covered = triangle.GetCoveredRectangle();
    const int x = covered.Left() - static_cast<int>(covered.width * 4.25);
    const int y = covered.Top() - covered.height;
    const int y2 = covered.Top() + static_cast<int>(covered.height * 1.2);
    if (x < 0 || y < 0) {
        return {{}, {}};
    }

    const Rectangle upper {x, y, covered.width * 7, static_cast<int>(covered.height * 0.8)};
    const Rectangle lower {x, y2, covered.width * 7, static_cast<int>(covered.height * 0.8)};
    return {Intersect(ImageRectangle(image), upper), Intersect(ImageRectangle(image), lower)};
}

bool VerifySkipAdBarPart(const cv::Mat& gray, const TriangleWithDescription& triangle, int color)
{
    const auto parts = OrientAsRightPointingTriangle(triangle);
    if (!parts.has_value()) {
        return false;
    }

    const int y1 = parts->upper.y + 1;
    const int y2 = parts->lower.y;
    const int height = y2 - y1;
    const int length = height / 2;
    if (height <= 0 || length <= 0 || parts->tip.x + length >= gray.cols || y1 < 0 || y2 > gray.rows) {
        return false;
    }

    std::vector<int> column_averages(static_cast<std::size_t>(length), 0);
    int max_column = 0;
    int max_value = 0;
    for (int i = 0, x = parts->tip.x + 1; i < length; ++i, ++x) {
        int sum = 0;
        for (int y = y1; y < y2; ++y) {
            sum += MatExtension::GetByteValue(gray, y, x);
        }
        const int average = sum / height;
        column_averages[static_cast<std::size_t>(i)] = average;
        if (average > max_value) {
            max_value = average;
            max_column = i;
        }
    }

    if (RelativeDifference(max_value, color) > 0.1 || max_column > 7 || max_column < 2) {
        return false;
    }

    int left_max = 0;
    int right_max = 0;
    for (left_max = max_column - 1; left_max > 0; --left_max) {
        if (RelativeDifference(column_averages[static_cast<std::size_t>(left_max)], max_value) > 0.1) {
            break;
        }
    }
    for (right_max = max_column + 1; right_max < length - 1; ++right_max) {
        if (RelativeDifference(column_averages[static_cast<std::size_t>(right_max)], max_value) > 0.1) {
            break;
        }
    }

    return right_max - left_max + 2 > static_cast<int>(length / 3.0);
}

bool ContainsTextBlockFromGray(const cv::Mat& gray, const SettingCanny& canny_setting, int min_width, float text_block_threshold)
{
    if (gray.empty()) {
        return false;
    }

    cv::Mat source {};
    if (gray.channels() == 1) {
        source = gray;
    } else {
        cv::cvtColor(gray, source, gray.channels() == 4 ? cv::COLOR_BGRA2GRAY : cv::COLOR_BGR2GRAY);
    }

    const int threshold1 = canny_setting.threshold1 > 0 ? canny_setting.threshold1 : 50;
    const int threshold2 = canny_setting.threshold2 > 0 ? canny_setting.threshold2 : 150;
    cv::Mat canny {};
    cv::Canny(source, canny, threshold1, threshold2, 3, canny_setting.l2_gradient);

    cv::Mat grad {};
    static const cv::Mat ellipse_kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
    cv::morphologyEx(canny, grad, cv::MORPH_GRADIENT, ellipse_kernel, cv::Point(-1, -1), 1, cv::BORDER_DEFAULT);

    cv::Mat bw {};
    cv::threshold(grad, bw, 0, 255, cv::THRESH_OTSU | cv::THRESH_BINARY);

    cv::Mat connected {};
    static const cv::Mat close_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(9, 3));
    cv::morphologyEx(bw, connected, cv::MORPH_CLOSE, close_kernel, cv::Point(-1, -1), 1, cv::BORDER_DEFAULT);

    std::vector<std::vector<cv::Point>> contours {};
    cv::findContours(connected, contours, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);
    const cv::Rect image_rect {0, 0, source.cols, source.rows};
    for (const auto& contour : contours) {
        const auto bounding = cv::boundingRect(contour) & image_rect;
        if (bounding.empty() || bounding.height <= 3 || bounding.height > 500 || bounding.width < min_width) {
            continue;
        }

        const double coverage =
            static_cast<double>(cv::countNonZero(grad(bounding)))
            / static_cast<double>(bounding.width * bounding.height);
        if (coverage > text_block_threshold) {
            return true;
        }
    }

    return false;
}

bool VerifySkipAdWordPart(const cv::Mat& gray, const SettingCanny& canny_setting, const TriangleWithDescription& triangle, int color, int delta)
{
    const Rectangle rect = ExpandSkipAdToSmallRectangleWord(gray, triangle);
    if (rect.IsEmpty()) {
        return false;
    }

    const auto cropped = CropMat(gray, rect);
    const auto filtered = OpenCvLib::FilterMatByColor(cropped, color, delta);
	return ContainsTextBlockFromGray(filtered, canny_setting, rect.height, 0.4F);
}

bool IsBoundColorBlack(const cv::Mat& gray, const TriangleWithDescription& triangle)
{
    const auto [upper, lower] = ExpandSkipAdToUpperAndDownRectangle(gray, triangle);
    if (upper.IsEmpty() || lower.IsEmpty()) {
        return false;
    }

    auto [color, coverage] = GetMaxColorInRectangle(gray, upper);
    if (color > 50 || coverage < 0.5F) {
        return false;
    }

    std::tie(color, coverage) = GetMaxColorInRectangle(gray, lower);
    return color <= 50 && coverage >= 0.5F;
}

bool VerifySkipAdByColor(const cv::Mat& gray, const TriangleWithDescription& triangle, const SettingCanny& canny_setting)
{
    const auto [color, coverage] = OpenCvLib::VerifySameCodeAndGetTheColor(gray, triangle.AsArray());
    if (coverage < 0.8F) {
        return false;
    }
    if (!VerifySkipAdBarPart(gray, triangle, color)) {
        return false;
    }
    if (!VerifySkipAdWordPart(gray, canny_setting, triangle, color, 10)) {
        return false;
    }
    return true;
}

bool VerifySkipAd(const cv::Mat& gray, const cv::Mat& canny, const TriangleWithDescription& triangle, const SettingCanny& canny_setting)
{
    if (!VerifySkipAdByColor(gray, triangle, canny_setting)) {
        return false;
    }
    return SkipAdDetector::VerifyNearCorner(canny, triangle);
}

std::vector<TriangleWithDescription> FindSkipAdInGray(
    const cv::Mat& gray,
    const SettingLineDetection& line_detection)
{
    if (gray.empty()) {
        return {};
    }

    const auto canny = OpenCvLib::ApplyCannyReturnRawIfFailed(
        gray,
        true,
        line_detection.setting_canny.threshold1,
        line_detection.setting_canny.threshold2);
    auto triangles = OpenCvLib::FindTriangles(canny);
    std::vector<TriangleWithDescription> result {};
    std::vector<Point> centers_processed {};
    result.reserve(triangles.size());
    centers_processed.reserve(triangles.size());
    constexpr int duplicate_delta = 4;

    for (const auto& triangle : triangles) {
        const auto center = triangle.Center();
        const bool is_close_to_processed = PointHelper::IsCloseToAny(center, centers_processed, duplicate_delta);
        centers_processed.push_back(center);
        if (is_close_to_processed) {
            continue;
        }

        if (VerifySkipAd(gray, canny, triangle, line_detection.setting_canny)) {
            result.push_back(triangle);
        }
    }
    return result;
}

std::optional<TriangleWithDescription> FindFirstSkipAdInGray(
    const cv::Mat& gray,
    const SettingLineDetection& line_detection)
{
    if (gray.empty()) {
        return std::nullopt;
    }

    const auto canny = OpenCvLib::ApplyCannyReturnRawIfFailed(
        gray,
        true,
        line_detection.setting_canny.threshold1,
        line_detection.setting_canny.threshold2);
    auto triangles = OpenCvLib::FindTriangles(canny);
    std::vector<Point> centers_processed {};
    centers_processed.reserve(triangles.size());
    constexpr int duplicate_delta = 4;

    for (const auto& triangle : triangles) {
        const auto center = triangle.Center();
        const bool is_close_to_processed = PointHelper::IsCloseToAny(center, centers_processed, duplicate_delta);
        centers_processed.push_back(center);
        if (is_close_to_processed) {
            continue;
        }

        if (VerifySkipAd(gray, canny, triangle, line_detection.setting_canny)) {
            return triangle;
        }
    }
    return std::nullopt;
}

} // namespace

std::vector<TriangleWithDescription> SkipAdDetector::FindSkipAd(const Bitmap& image, const SettingLineDetection& line_detection)
{
    const auto gray = BitmapRegionToGrayMat(image, Rectangle {0, 0, image.width, image.height});
    return FindSkipAdInGray(gray, line_detection);
}

void SkipAdDetector::RegisterBindings(automationtest::utilities::status::LoadFunctions& load_functions)
{
    using Method = automationtest::utilities::status::LoadFunctions::RegisteredMethod;
    using Parameter = automationtest::utilities::status::LoadFunctions::RegisteredParameter;

    load_functions.RegisterMethod("ClickOnSkipAd", Method {
        .name = "ClickOnSkipAd",
        .declaring_type = "SkipAdDetector",
        .invoke = [](const std::vector<std::any>& arguments) -> std::any {
            const std::any empty {};
            const auto& bitmaps = arguments.empty() ? empty : arguments[0];
            const auto& line_detection = arguments.size() < 2 ? empty : arguments[1];
            return SkipAdDetector::ClickOnSkipAd(bitmaps, line_detection);
        },
        .parameters = {
            Parameter {.name = "bitmaps"},
            Parameter {.name = "lineDetection"},
        },
    });
}

std::any SkipAdDetector::ClickOnSkipAd(const std::any& bitmaps, const std::any& line_detection)
{
    const std::vector<LocatedBitmap>* bitmaps_pointer = std::any_cast<std::vector<LocatedBitmap>>(&bitmaps);
    if (bitmaps_pointer == nullptr) {
        if (const auto* shared_bitmaps = std::any_cast<std::shared_ptr<std::vector<LocatedBitmap>>>(&bitmaps);
            shared_bitmaps != nullptr && *shared_bitmaps != nullptr) {
            bitmaps_pointer = shared_bitmaps->get();
        }
    }
    if (bitmaps_pointer == nullptr) {
        return {};
    }

    SettingLineDetection setting {};
    if (const auto* resolved_setting = std::any_cast<SettingLineDetection>(&line_detection); resolved_setting != nullptr) {
        setting = *resolved_setting;
    } else if (line_detection.has_value()) {
        throw TestException("Cannot find SettingLineDetection");
    }

    const ClickOnlyCallback click = [](const Point& point) {
        MouseKeyboardLib::ClickOnPointAndRestoreForegroundWindow(point);
    };
    return ClickOnSkipAd(*bitmaps_pointer, click, setting);
}

std::optional<Bitmap> SkipAdDetector::ClickOnSkipAd(const std::vector<LocatedBitmap>& bitmaps, const ClickOnlyCallback& mouse_click, const SettingLineDetection& line_detection)
{
    if (!mouse_click) {
        return std::nullopt;
    }
    if (bitmaps.empty()) {
        Logger::LogToView("ClickOnSkipAd received no screenshots.");
        return std::nullopt;
    }

    for (const auto& [image, location] : bitmaps) {
        if (image.width <= 0 || image.height <= 0 || image.pixels.empty()) {
            Logger::LogToView(
                "Skipping empty screenshot bitmap. width="
                + std::to_string(image.width)
                + ", height="
                + std::to_string(image.height)
                + ", pixels="
                + std::to_string(image.pixels.size()));
            continue;
        }

        const auto gray = BitmapRegionToGrayMat(image, Rectangle {0, 0, image.width, image.height});
        const auto triangle = FindFirstSkipAdInGray(gray, line_detection);
        if (!triangle.has_value()) {
            continue;
        }

        const auto center = triangle->Center();
        Logger::LogToView( "Found skip ad triangle at (" + std::to_string(center.x) + ", " + std::to_string(center.y) + ") in the " + BitmapHelper::DebugSave(image) + " screenshot." );
        mouse_click(Point {center.x + location.x, center.y + location.y});
        return std::nullopt;
    }

    return std::nullopt;
}

bool SkipAdDetector::VerifyNearCorner(const cv::Mat& cannied, const TriangleWithDescription& triangle, int kernel_w, int kernel_h)
{
    const auto rect = triangle.GetCoveredRectangle();
    if (rect.IsEmpty() || cannied.empty()) {
        return false;
    }

    const auto center = triangle.Center();
    const int height = rect.height;
    if (height <= 0) {
        return false;
    }

    bool near_bottom = center.y > cannied.rows - height * 9;
    bool near_right = center.x > cannied.cols - height * 5;
    const int right_range_from = center.x + height * 2;
    const int right_range_to = center.x + height * 5;
    const int y_range_from = center.y + height;
    const int y_range_to = center.y + height * 9;

    if (near_bottom && near_right) {
        return true;
    }

    auto [vertical_lines, horizon_lines] = OpenCvLib::FindLinesAfterCannyHorizonAndVerticalLines(cannied, height * 3);

    if (!near_right) {
        for (const auto& vertical : vertical_lines) {
            if (vertical.x1 < right_range_from || vertical.x1 > right_range_to) {
                continue;
            }
            if (vertical.y1 < center.y && vertical.y2 > center.y) {
                near_right = true;
                break;
            }
        }

        if (!near_right) {
            const int top_from = std::max(center.y - height * 15, 1);
            const Rectangle roi {right_range_from, top_from, right_range_to - right_range_from, center.y - top_from + height};
            const auto cropped = CropMat(cannied, Intersect(ImageRectangle(cannied), roi));
            if (!cropped.empty()) {
                near_right = StripeDetector::DetectIfStripExists(cropped, 7, std::min(300, std::max(1, cropped.rows - height)));
            }
        }
    }

    if (!near_right)
        return false;

    if (!near_bottom) {
        for (const auto& horizon : horizon_lines) {
            if (horizon.y1 < y_range_from || horizon.y1 > y_range_to) {
                continue;
            }
            if (horizon.x1 < center.x && horizon.x2 > center.x) {
                near_bottom = true;
                break;
            }
        }

        if (!near_bottom) {
            const int left_from = std::max(center.x - height * 20, 1);
            const Rectangle roi {left_from, y_range_from, center.x - left_from, y_range_to - y_range_from};
            const auto cropped = CropMat(cannied, Intersect(ImageRectangle(cannied), roi));
            if (!cropped.empty()) {
                near_bottom = StripeDetector::DetectIfStripExists(cropped, std::min(300, std::max(1, cropped.cols - height)), 7);
            }
        }
    }

    return near_bottom && near_right;
}

} // namespace automationtest::userinterfacelib
