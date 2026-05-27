#include "OpenCvLib.hpp"

#include "LineDetector.hpp"
#include "LineSegment2DHelper.hpp"
#include "MatExtension.hpp"
#include "RectangleDetector.hpp"
#include "SmallTriangleDetector.hpp"

#include "../Utilities/MathLib.hpp"
#include "../Utilities/BitmapHelper.hpp"
#include "../Utilities/Logger.hpp"
#include "../Utilities/GlobalSetting.hpp"
#include "../Utilities/IdGenerator.hpp"

#include <algorithm>
#include <cmath>
#include <cctype>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <limits>
#include <optional>
#include <set>
#include <string>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace automationtest::opencvlib {

using automationtest::utilities::Bitmap;
using automationtest::utilities::GlobalSetting;
using automationtest::utilities::IdGenerator;
using automationtest::utilities::LineWithDescription;
using automationtest::utilities::Logger;
using automationtest::utilities::MathLib;
using automationtest::utilities::Point;
using automationtest::utilities::Rectangle;
using automationtest::utilities::TriangleWithDescription;
using automationtest::utilities::settings::SettingCanny;
using automationtest::utilities::settings::SettingLineDetection;
using automationtest::utilities::settings::SettingLoadMatGray;
using automationtest::utilities::settings::SettingRectangleDetection;
using automationtest::utilities::settings::SettingThreshold;
using automationtest::utilities::types::ShortLine;

namespace {

std::string NormalizeThresholdName(std::string value)
{
    value.erase(std::remove_if(value.begin(), value.end(), [](unsigned char ch) {
        return std::isspace(ch) != 0 || ch == '_' || ch == '-';
    }), value.end());
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

std::optional<int> ParseThresholdType(const std::string& threshold_type)
{
    const auto name = NormalizeThresholdName(threshold_type);
    if (name.empty() || name == "none") {
        return std::nullopt;
    }
    if (name == "binary") {
        return cv::THRESH_BINARY;
    }
    if (name == "binaryinv") {
        return cv::THRESH_BINARY_INV;
    }
    if (name == "trunc") {
        return cv::THRESH_TRUNC;
    }
    if (name == "tozero") {
        return cv::THRESH_TOZERO;
    }
    if (name == "tozeroinv") {
        return cv::THRESH_TOZERO_INV;
    }
    if (name == "otsu") {
        return cv::THRESH_BINARY | cv::THRESH_OTSU;
    }
    if (name == "triangle") {
        return cv::THRESH_BINARY | cv::THRESH_TRIANGLE;
    }
    return std::nullopt;
}

int NormalizeApertureSize(int aperture_size)
{
    if (aperture_size == 3 || aperture_size == 5 || aperture_size == 7) {
        return aperture_size;
    }
    return 3;
}

cv::Mat ApplyCannyEffect(const cv::Mat& mat, const SettingCanny& canny)
{
    if (mat.empty()) {
        return {};
    }

    const double threshold1 = canny.threshold1 > 0 ? canny.threshold1 : 100.0;
    const double threshold2 = canny.threshold2 > 0 ? canny.threshold2 : 200.0;
    cv::Mat edges {};
    cv::Canny(mat, edges, threshold1, threshold2, NormalizeApertureSize(canny.aperture_size), canny.l2_gradient);
    return edges;
}

Point ToPoint(const cv::Point& point)
{
    return Point {point.x, point.y};
}

Rectangle ToRectangle(const cv::Rect& rect)
{
    return Rectangle {rect.x, rect.y, rect.width, rect.height};
}

bool BetweenInclusive(int value, int minimum, int maximum) noexcept
{
    return value >= minimum && value <= maximum;
}

int Area(const Rectangle& rectangle) noexcept
{
    if (rectangle.width <= 0 || rectangle.height <= 0) {
        return 0;
    }
    if (rectangle.width > std::numeric_limits<int>::max() / rectangle.height) {
        return std::numeric_limits<int>::max();
    }
    return rectangle.width * rectangle.height;
}

int EdgeFunction(const Point& a, const Point& b, const Point& c) noexcept
{
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

bool PointInTriangle(const Point& point, const std::vector<Point>& triangle) noexcept
{
    const int edge1 = EdgeFunction(triangle[0], triangle[1], point);
    const int edge2 = EdgeFunction(triangle[1], triangle[2], point);
    const int edge3 = EdgeFunction(triangle[2], triangle[0], point);
    const bool has_negative = edge1 < 0 || edge2 < 0 || edge3 < 0;
    const bool has_positive = edge1 > 0 || edge2 > 0 || edge3 > 0;
    return !(has_negative && has_positive);
}

std::vector<LineWithDescription> HoughLinesToDescriptions(const std::vector<cv::Vec4i>& lines)
{
    std::vector<LineWithDescription> result {};
    result.reserve(lines.size());
    for (const auto& line : lines) {
        result.emplace_back(Point {line[0], line[1]}, Point {line[2], line[3]});
    }
    return result;
}

} // namespace

cv::Mat OpenCvLib::ToGrayMat(const Bitmap& bitmap)
{
    const auto source = MatExtension::BitmapToMat(bitmap);
    if (source.empty()) {
        return {};
    }

    cv::Mat gray {};
    if (source.channels() == 1) {
        gray = source.clone();
    } else if (source.channels() == 3) {
        cv::cvtColor(source, gray, cv::COLOR_BGR2GRAY);
    } else if (source.channels() == 4) {
        cv::cvtColor(source, gray, cv::COLOR_BGRA2GRAY);
    } else {
        gray = source.reshape(1).clone();
    }
    return gray;
}

std::string OpenCvLib::InfoSaveMatAsBitmapFile(const cv::Mat& mat)
{
    if (!Logger::IsAboveInfo()) {
        return {};
    }
    return DebugSaveMatAsBitmapFile(mat);
}

std::string OpenCvLib::DebugSaveMatAsBitmapFile(const cv::Mat& mat )
{
    if (!GlobalSetting::save_bitmap_files && !Logger::IsAboveDebug()) {
        return {};
    }

    if (mat.empty()) {
        return {};
    }

    std::string filename = "mat_" + IdGenerator::IdWithDateTime() + ".bmp";
    std::filesystem::path output_path(GlobalSetting::ImageFileFolder());
    output_path /= filename;

	return SaveMatAsBitmapFile(mat, output_path.string());
}

std::string OpenCvLib::SaveMatAsBitmapFile(const cv::Mat& mat, const std::string& filename)
{
    if (mat.empty() || filename.empty()) {
        return {};
    }

    try {
        std::filesystem::path output_path(filename);
        output_path.replace_extension(".bmp");

        if (const auto parent_path = output_path.parent_path(); !parent_path.empty()) {
            std::filesystem::create_directories(parent_path);
        }

        cv::Mat output;
        if (mat.depth() == CV_8U) {
            output = mat;
        } else {
            cv::normalize(mat, output, 0, 255, cv::NORM_MINMAX);
            output.convertTo(output, CV_8U);
        }
		std::string result = output_path.string();
        cv::imwrite(result, output);
		return result;

    } catch (const cv::Exception&) {
        return {};
    } catch (const std::exception&) {
        return {};
    }
}

cv::Mat OpenCvLib::ApplyApplyThresholdReturnRawIfFailed(const cv::Mat& mat, const SettingLoadMatGray& gray)
{
    SettingThreshold threshold {};
    threshold.apply_threshold = gray.apply_threshold;
    threshold.threshold_value = gray.threshold_value;
    threshold.threshold_max_value = gray.threshold_max_value;
    return ApplyApplyThresholdReturnRawIfFailed(mat, threshold);
}

cv::Mat OpenCvLib::ApplyApplyThresholdReturnRawIfFailed(const cv::Mat& mat, const SettingThreshold& threshold)
{
    if (mat.empty()) {
        return {};
    }

    if (const auto threshold_type = ParseThresholdType(threshold.apply_threshold); threshold_type.has_value()) {
        cv::Mat thresholded {};
        cv::threshold(mat, thresholded, threshold.threshold_value, threshold.threshold_max_value, *threshold_type);
        return thresholded;
    }

    if (mat.channels() == 1) {
        return mat.clone();
    }

    cv::Mat gray {};
    if (mat.channels() == 3) {
        cv::cvtColor(mat, gray, cv::COLOR_BGR2GRAY);
    } else if (mat.channels() == 4) {
        cv::cvtColor(mat, gray, cv::COLOR_BGRA2GRAY);
    } else {
        gray = mat.reshape(1).clone();
    }
    return gray;
}

cv::Mat OpenCvLib::ApplyCannyReturnRawIfFailed(const cv::Mat& mat, bool apply_canny, double canny_threshold1, double canny_threshold2)
{
    if (!apply_canny || mat.empty()) {
        return mat.clone();
    }

    SettingCanny setting {};
    setting.threshold1 = static_cast<int>(canny_threshold1);
    setting.threshold2 = static_cast<int>(canny_threshold2);
    setting.aperture_size = 3;
    setting.l2_gradient = true;
    return ApplyCannyEffect(mat, setting);
}

std::vector<LineWithDescription> OpenCvLib::FindLines(const cv::Mat& img, int min_len, int max_gap)
{
    if (img.empty()) {
        return {};
    }

    std::vector<cv::Vec4i> hough_lines {};
    cv::HoughLinesP(img, hough_lines, 1.0, CV_PI / 180.0, 2, min_len, max_gap);
    return HoughLinesToDescriptions(hough_lines);
}

std::vector<LineWithDescription> OpenCvLib::FindLines(const Bitmap& image, const SettingLineDetection& setting)
{
    const auto gray = ToGrayMat(image);
    auto intermediate = ApplyApplyThresholdReturnRawIfFailed(gray, setting.setting_threshold);
    intermediate = ApplyCannyEffect(intermediate, setting.setting_canny);
    return FindLines(intermediate, setting.hough_lines_min_line_length, setting.hough_lines_max_line_gap);
}

std::vector<LineSegment2D> OpenCvLib::FindLinesReturnLineSegment2D(const Bitmap& image, const SettingLineDetection& setting)
{
    std::vector<LineSegment2D> result {};
    for (const auto& line : FindLines(image, setting)) {
        result.push_back(LineSegment2D {line.Point1(), line.Point2()});
    }
    return result;
}

std::pair<std::vector<ShortLine>, std::vector<ShortLine>> OpenCvLib::FindLinesAfterCannyHorizonAndVerticalLines(const cv::Mat& gray, int min_length, int max_line_gap)
{
    if (gray.empty()) {
        return {{}, {}};
    }

    std::vector<ShortLine> verticals {};
    std::vector<ShortLine> horizontals {};
    std::vector<cv::Vec4i> hough_lines {};
    cv::HoughLinesP(
        gray,
        hough_lines,
        1.0,
        CV_PI / 4.0,
        min_length,
        min_length,
        max_line_gap);

    for (const auto& raw_line : hough_lines) {
        const auto line = LineWithDescription {
            Point {raw_line[0], raw_line[1]},
            Point {raw_line[2], raw_line[3]}};
        if (line.IsVerticalLine()) {
            verticals.push_back(LineSegment2DHelper::ToShortPoint(LineSegment2D {line.Point1(), line.Point2()}));
        } else if (line.IsHorizonLine()) {
            horizontals.push_back(LineSegment2DHelper::ToShortPoint(LineSegment2D {line.Point1(), line.Point2()}));
        }
    }
    return {verticals, horizontals};
}

std::vector<Rectangle> OpenCvLib::FindHorizonAndVerticalRectangles(const cv::Mat& gray, const SettingRectangleDetection* setting)
{
    if (gray.empty()) {
        return {};
    }

    const auto local_setting = setting != nullptr ? *setting : SettingRectangleDetection {};
    cv::Mat canny_edges {};
    cv::Canny(gray, canny_edges, local_setting.canny_threshold, local_setting.canny_threshold_linking);

    std::vector<std::vector<cv::Point>> contours {};
    cv::findContours(canny_edges, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    std::set<std::tuple<int, int, int, int>> unique {};
    std::vector<Rectangle> rectangles {};
    for (const auto& contour : contours) {
        const auto bounding = cv::boundingRect(contour);
        const auto rectangle = ToRectangle(bounding);
        if (!BetweenInclusive(rectangle.width, local_setting.min_width, local_setting.max_width)
            || !BetweenInclusive(rectangle.height, local_setting.min_height, local_setting.max_height)
            || !BetweenInclusive(Area(rectangle), local_setting.min_area, local_setting.max_area)) {
            continue;
        }

        double epsilon = local_setting.epsilon;
        if (epsilon <= 0.0) {
            epsilon = 5.0;
        } else if (epsilon <= 0.5) {
            epsilon *= cv::arcLength(contour, true);
        }

        std::vector<cv::Point> approx {};
        cv::approxPolyDP(contour, approx, epsilon, true);
        if (approx.size() == 4) {
            const auto key = std::make_tuple(rectangle.x, rectangle.y, rectangle.width, rectangle.height);
            if (unique.insert(key).second) {
                rectangles.push_back(rectangle);
            }
        }
    }
    return rectangles;
}

std::vector<Rectangle> OpenCvLib::FindHorizonAndVerticalRectanglesUsingLines(const Bitmap& bitmap, const SettingRectangleDetection* setting)
{
    const auto local_setting = setting != nullptr ? *setting : SettingRectangleDetection {};
    const auto line_setting = SettingLineDetection {};
    auto lines = FindLines(bitmap, line_setting);
    return RectangleDetector().ExtractRectanglesFromLines(lines, local_setting.point_merge_delta, local_setting.point_merge_delta);
}

std::vector<LineWithDescription> OpenCvLib::FindHorizonVerticalLines(const cv::Mat& gray, const SettingLineDetection* setting)
{
    if (gray.empty()) {
        return {};
    }

    const auto local_setting = setting != nullptr ? *setting : SettingLineDetection {};
    const auto canny_edges = ApplyCannyEffect(gray, local_setting.setting_canny);

    std::vector<cv::Vec4i> hough_lines {};
    cv::HoughLinesP(canny_edges,
        hough_lines,
        1.0,
        local_setting.angle_resolution_in_radians,
        local_setting.hough_lines_threshold,
        local_setting.hough_lines_min_line_length,
        local_setting.hough_lines_max_line_gap);

    std::vector<cv::Vec4i> hough_lines_90_degree {};
    cv::HoughLinesP(canny_edges,
        hough_lines_90_degree,
        1.0,
        CV_PI / 2.0,
        local_setting.hough_lines_threshold,
        local_setting.hough_lines_min_line_length,
        local_setting.hough_lines_max_line_gap);

    hough_lines.insert(hough_lines.end(), hough_lines_90_degree.begin(), hough_lines_90_degree.end());
    auto lines = HoughLinesToDescriptions(hough_lines);
    std::sort(lines.begin(), lines.end(), [](const LineWithDescription& left, const LineWithDescription& right) {
        if (left.Point1().y != right.Point1().y) {
            return left.Point1().y < right.Point1().y;
        }
        return left.Point1().x < right.Point1().x;
    });
    return lines;
}

std::vector<TriangleWithDescription> OpenCvLib::FindTriangles(const cv::Mat& cannied)
{
    if (cannied.empty()) {
        return {};
    }

    std::vector<std::vector<cv::Point>> contours {};
    cv::findContours(cannied, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    std::vector<TriangleWithDescription> triangles {};
    for (const auto& contour : contours) {
        if (contour.size() < 3) {
            continue;
        }

        const auto bounding = cv::boundingRect(contour);
        if (bounding.width > 60 || bounding.height > 60 || bounding.width * bounding.height <= 90) {
            continue;
        }

        std::vector<cv::Point> approx {};
        const auto epsilon = std::max(cv::arcLength(contour, true) * 0.05, 6.0);
        cv::approxPolyDP(contour, approx, epsilon, true);
        if (approx.size() != 3 || cv::contourArea(approx) <= 90.0) {
            continue;
        }

        std::array<Point, 3> points {ToPoint(approx[0]), ToPoint(approx[1]), ToPoint(approx[2])};
        std::sort(points.begin(), points.end(), [](const Point& left, const Point& right) {
            if (left.x != right.x) {
                return left.x < right.x;
            }
            return left.y < right.y;
        });

        if (points[1].x - points[0].x >= 4) {
            continue;
        }
        if (std::abs(points[2].y * 2 - points[0].y - points[1].y) >= 6) {
            continue;
        }

        const int height = points[1].y - points[0].y;
        const int width = std::abs(points[2].x - points[0].x);
        if (height > 50 || width > 50) {
            continue;
        }
        if (std::abs(height - width) < 8) {
            triangles.emplace_back(points[0], points[1], points[2]);
        }
    }
    return triangles;
}

std::vector<TriangleWithDescription> OpenCvLib::FindSmallTriangles(const Bitmap& bitmap)
{
    std::vector<TriangleWithDescription> triangles {};
    for (const auto& rectangle : SmallTriangleDetector(bitmap).FindSmallTriangles()) {
        const auto top = rectangle.y;
        const auto bottom = rectangle.y + rectangle.height - 1;
        const auto left = rectangle.x;
        const auto right = rectangle.x + rectangle.width - 1;
        triangles.emplace_back(Point {left, top}, Point {left, bottom}, Point {right, rectangle.y + rectangle.height / 2});
    }
    return triangles;
}

std::pair<int, float> OpenCvLib::VerifySameCodeAndGetTheColor(const cv::Mat& gray, const std::vector<Point>& points)
{
    if (gray.empty() || points.size() < 3) {
        return {0, 0.0F};
    }

    int min_x = std::min({points[0].x, points[1].x, points[2].x});
    int max_x = std::max({points[0].x, points[1].x, points[2].x});
    int min_y = std::min({points[0].y, points[1].y, points[2].y});
    int max_y = std::max({points[0].y, points[1].y, points[2].y});
    min_x = std::max(0, min_x);
    min_y = std::max(0, min_y);
    max_x = std::min(gray.cols - 1, max_x);
    max_y = std::min(gray.rows - 1, max_y);

    std::vector<int> histogram(256, 0);
    int total = 0;
    for (int row = min_y; row <= max_y; ++row) {
        for (int col = min_x; col <= max_x; ++col) {
            if (!PointInTriangle(Point {col, row}, points)) {
                continue;
            }
            ++histogram[MatExtension::GetByteValue(gray, row, col)];
            ++total;
        }
    }

    if (total == 0) {
        return {0, 0.0F};
    }

    const auto group = MathLib::GetMostConcentratedGroup(histogram);
    return {static_cast<int>(group.weighted_avg), group.total_count / static_cast<float>(total)};
}

std::vector<Rectangle> OpenCvLib::FindTextBlocksFromGray(const cv::Mat& gray, const SettingCanny& canny_setting, int min_width, float text_block_threshold)
{
    if (gray.empty()) {
        return {};
    }

    cv::Mat source {};
    if (gray.channels() == 1) {
        source = gray;
    } else {
        cv::cvtColor(gray, source, gray.channels() == 4 ? cv::COLOR_BGRA2GRAY : cv::COLOR_BGR2GRAY);
    }

    cv::Mat canny {};
    cv::Mat grad {};
    cv::Mat bw {};
    cv::Mat connected {};
    const int threshold1 = canny_setting.threshold1 > 0 ? canny_setting.threshold1 : 50;
    const int threshold2 = canny_setting.threshold2 > 0 ? canny_setting.threshold2 : 150;
    cv::Canny(source, canny, threshold1, threshold2, NormalizeApertureSize(canny_setting.aperture_size), canny_setting.l2_gradient);

    auto morph_kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3), cv::Point(-1, -1));
    cv::morphologyEx(canny, grad, cv::MORPH_GRADIENT, morph_kernel, cv::Point(-1, -1), 1, cv::BORDER_DEFAULT);

    cv::threshold(grad, bw, 0, 255, cv::THRESH_OTSU | cv::THRESH_BINARY);

    morph_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(9, 3), cv::Point(-1, -1));
    cv::morphologyEx(bw, connected, cv::MORPH_CLOSE, morph_kernel, cv::Point(-1, -1), 1, cv::BORDER_DEFAULT);

    std::vector<std::vector<cv::Point>> contours {};
    cv::findContours(connected, contours, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE, cv::Point(-1, -1));

    std::set<std::tuple<int, int, int, int>> unique {};
    std::vector<Rectangle> rectangles {};
    const cv::Rect image_rect {0, 0, source.cols, source.rows};
    for (const auto& contour : contours) {
        std::vector<cv::Point> approx_contour {};
        cv::approxPolyDP(contour, approx_contour, cv::arcLength(contour, true) * 0.05, true);
        const auto bounding = cv::boundingRect(approx_contour);
        const auto clipped = bounding & image_rect;
        const auto rect = ToRectangle(clipped);
        if (clipped.empty() || bounding.height <= 3 || rect.height > 500 || rect.width < min_width) {
            continue;
        }

        const auto mask_roi = grad(clipped);
        const double coverage = static_cast<double>(cv::countNonZero(mask_roi)) / static_cast<double>(rect.width * rect.height);
        if (coverage <= text_block_threshold) {
            continue;
        }

        const Rectangle result {rect.x, rect.y, rect.width + 1, rect.height + 1};
        const auto key = std::make_tuple(result.x, result.y, result.width, result.height);
        if (unique.insert(key).second) {
            rectangles.push_back(result);
        }
    }

    std::sort(rectangles.begin(), rectangles.end(), [](const Rectangle& left, const Rectangle& right) {
        if (left.y != right.y) {
            return left.y < right.y;
        }
        return left.x < right.x;
    });
    return rectangles;
}
cv::Mat OpenCvLib::FilterMatByColor(const cv::Mat& gray, int color, int delta)
{
    if (gray.empty()) {
        return {};
    }

    cv::Mat upper_limited {};
    cv::threshold(gray, upper_limited, color + delta, 255, cv::THRESH_TOZERO_INV);
    cv::Mat lower_limited {};
    cv::threshold(upper_limited, lower_limited, color - delta, 255, cv::THRESH_TOZERO);
    return lower_limited;
}

} // namespace automationtest::opencvlib

namespace {

#if defined(_WIN32)
std::string ShellQuote(const std::string& value)
{
    std::string result {"\""};
    for (const char ch : value) {
        if (ch == '"') {
            result += "\\\"";
        } else {
            result += ch;
        }
    }
    result += '"';
    return result;
}
#else
std::string ShellQuote(const std::string& value)
{
    std::string result {"'"};
    for (const char ch : value) {
        if (ch == '\'') {
            result += "'\\''";
        } else {
            result += ch;
        }
    }
    result += '\'';
    return result;
}
#endif

std::filesystem::path DebugImageFilePath(const std::string& prefix)
{
    std::filesystem::path folder(automationtest::utilities::GlobalSetting::DebugViewImageFileFolder());
    if (folder.empty()) {
        folder = std::filesystem::current_path();
    }
    return folder / (prefix + "_" + automationtest::utilities::IdGenerator::IdWithDateTime() + ".bmp");
}

void StartBitmapViewer(const std::string& filename, const std::string& log_category)
{
    const auto tool = automationtest::utilities::GlobalSetting::ToolsToViewBitmap();
    std::string command;

#if defined(_WIN32)
    if (tool.empty()) {
        command = "start \"\" " + ShellQuote(filename);
    } else {
        command = "start \"\" " + ShellQuote(tool) + " " + ShellQuote(filename);
    }
#elif defined(__APPLE__)
    if (tool.empty()) {
        command = "open " + ShellQuote(filename) + " >/dev/null 2>&1 &";
    } else {
        command = ShellQuote(tool) + " " + ShellQuote(filename) + " >/dev/null 2>&1 &";
    }
#else
    if (tool.empty()) {
        command = "xdg-open " + ShellQuote(filename) + " >/dev/null 2>&1 &";
    } else {
        command = ShellQuote(tool) + " " + ShellQuote(filename) + " >/dev/null 2>&1 &";
    }
#endif

    const int result = std::system(command.c_str());
    if (result != 0) {
        automationtest::utilities::Logger::Error("Failed to start bitmap viewer for " + filename, log_category);
    }
}

} // namespace

void DM(cv::Mat& mat)
{
    if (mat.empty()) {
        automationtest::utilities::Logger::Error("Cannot display an empty cv::Mat.", "DM");
        return;
    }

    const auto filename = DebugImageFilePath("DM").string();
    const auto saved_file = automationtest::opencvlib::OpenCvLib::SaveMatAsBitmapFile(mat, filename);
    if (saved_file.empty()) {
        automationtest::utilities::Logger::Error("Failed to save debug cv::Mat bitmap.", "DM");
        return;
    }

    StartBitmapViewer(saved_file, "DM");
}

void DB(const Bitmap& bp)
{
    const auto filename = DebugImageFilePath("DB").string();
    const auto saved_file = automationtest::utilities::BitmapHelper::SaveBitmapFile(bp, filename);
    if (saved_file.empty()) {
        automationtest::utilities::Logger::Error("Failed to save debug Bitmap.", "DB");
        return;
    }

    StartBitmapViewer(saved_file, "DB");
}
