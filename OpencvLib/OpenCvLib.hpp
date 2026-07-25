#ifndef AUTOMATIOTEST_OPENCVLIB_OPENCVLIB_HPP
#define AUTOMATIOTEST_OPENCVLIB_OPENCVLIB_HPP

#include "LineSegment2D.hpp"
#include "../Utilities/Settings/SettingCanny.hpp"
#include "../Utilities/Settings/SettingLineDetection.hpp"
#include "../Utilities/Settings/SettingLoadMatGray.hpp"
#include "../Utilities/Settings/SettingRectangleDetection.hpp"
#include "../Utilities/Settings/SettingThreshold.hpp"

#include "../Utilities/CommonTypes.hpp"
#include "../Utilities/LineWithDescription.hpp"
#include "../Utilities/RectanglePair.hpp"
#include "../Utilities/RectangleToRectangleList.hpp"
#include "../Utilities/TriangleWithDescription.hpp"
#include "../Utilities/Types/ShortLine.hpp"

#include <string>
#include <utility>
#include <vector>

#include <opencv2/core.hpp>

namespace automationtest::opencvlib {

class OpenCvLib {
public:
    static cv::Mat ToGrayMat(const automationtest::utilities::Bitmap& bitmap);
    static std::string DebugSaveMatAsBitmapFile(const cv::Mat& mat);
    static std::string InfoSaveMatAsBitmapFile(const cv::Mat& mat);
    static std::string SaveMatAsBitmapFile(const cv::Mat& mat, const std::string& filename);
    static cv::Mat ApplyThresholdReturnRawIfFailed(const cv::Mat& mat, const automationtest::utilities::settings::SettingLoadMatGray& gray);
    static cv::Mat ApplyThresholdReturnRawIfFailed(const cv::Mat& mat, const automationtest::utilities::settings::SettingThreshold& threshold);
    static cv::Mat ApplyCannyReturnRawIfFailed(const cv::Mat& mat, bool apply_canny, double canny_threshold1, double canny_threshold2);
    static std::vector<automationtest::utilities::LineWithDescription> FindLines(const cv::Mat& img, int min_len = 100, int max_gap = 2);
    static std::vector<automationtest::utilities::LineWithDescription> FindLines(const automationtest::utilities::Bitmap& image, const automationtest::utilities::settings::SettingLineDetection& setting);
    static std::vector<LineSegment2D> FindLinesReturnLineSegment2D(const automationtest::utilities::Bitmap& image, const automationtest::utilities::settings::SettingLineDetection& setting);
    static std::pair<std::vector<automationtest::utilities::types::ShortLine>, std::vector<automationtest::utilities::types::ShortLine>> FindLinesAfterCannyHorizonAndVerticalLines(const cv::Mat& gray, int min_length, int max_line_gap = 4);
    static std::vector<automationtest::utilities::Rectangle> FindHorizonAndVerticalRectangles(const cv::Mat& gray, const automationtest::utilities::settings::SettingRectangleDetection* setting = nullptr);
    static std::vector<automationtest::utilities::Rectangle> FindHorizonAndVerticalRectanglesUsingLines(const automationtest::utilities::Bitmap& bitmap, const automationtest::utilities::settings::SettingRectangleDetection* setting = nullptr);
    static std::vector<automationtest::utilities::LineWithDescription> FindHorizonVerticalLines(const cv::Mat& gray, const automationtest::utilities::settings::SettingLineDetection* setting = nullptr);
    static std::vector<automationtest::utilities::TriangleWithDescription> FindTriangles(const cv::Mat& cannied);
    static std::vector<automationtest::utilities::TriangleWithDescription> FindSmallTriangles(const automationtest::utilities::Bitmap& bitmap);
    static std::vector<automationtest::utilities::Rectangle> FindTextBlocksFromGray(const cv::Mat& gray, const automationtest::utilities::settings::SettingCanny& canny_setting, int min_width, float text_block_threshold = 0.3F);
    static std::pair<int, float> VerifySameCodeAndGetTheColor(const cv::Mat& gray, const std::vector<automationtest::utilities::Point>& points);
    static cv::Mat FilterMatByColor(const cv::Mat& gray, int color, int delta);
    static std::string SerializeAsJson(std::vector<std::vector<cv::Point>> contours);
};

} // namespace automationtest::opencvlib

using Bitmap = automationtest::utilities::Bitmap;

const char* DM(const cv::Mat& mat);
const char* DB(const Bitmap& bp);
const char* DP(std::vector<std::vector<cv::Point>>& contours);

#endif // AUTOMATIOTEST_OPENCVLIB_OPENCVLIB_HPP
