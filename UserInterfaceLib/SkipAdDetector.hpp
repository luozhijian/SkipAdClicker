#ifndef AUTOMATIOTEST_USERINTERFACELIB_SKIPADDETECTOR_HPP
#define AUTOMATIOTEST_USERINTERFACELIB_SKIPADDETECTOR_HPP

#include "../Utilities/Common.hpp"
#include <opencv2/core.hpp>
#include "../Utilities/Settings/SettingLineDetection.hpp"
#include "../Utilities/TriangleWithDescription.hpp"

#include <optional>
#include <utility>
#include <vector>

namespace automationtest::userinterfacelib {

class SkipAdDetector {
public:
    using LocatedBitmap = std::pair<Bitmap, automationtest::utilities::Point>;
    static void RegisterBindings(automationtest::utilities::status::LoadFunctions& load_functions);
    static std::any ClickOnSkipAd(const std::any& bitmaps, const std::any& line_detection);
    static bool VerifyNearCorner(const cv::Mat& cannied, const automationtest::utilities::TriangleWithDescription& triangle, int kernel_w = 300, int kernel_h = 7);

private:
    static std::vector<automationtest::utilities::TriangleWithDescription> FindSkipAd(const Bitmap& image, const automationtest::utilities::settings::SettingLineDetection& line_detection = {});
    static std::optional<Bitmap> ClickOnSkipAd(const std::vector<LocatedBitmap>& bitmaps, const ClickOnlyCallback& mouse_click, const automationtest::utilities::settings::SettingLineDetection& line_detection = {});
};

} // namespace automationtest::userinterfacelib

#endif // AUTOMATIOTEST_USERINTERFACELIB_SKIPADDETECTOR_HPP
