#ifndef AUTOMATIOTEST_BITMAPHELPER_HPP
#define AUTOMATIOTEST_BITMAPHELPER_HPP

#include "CommonTypes.hpp"

#include <optional>
#include <string>
#include <vector>

namespace automationtest::utilities {

class BitmapHelper {
public:
    static Bitmap ResizeImage(const Bitmap& bitmap, const Size& size);
    static Bitmap CreateGrayscaleBitmap(const std::vector<std::byte>& pixel_data, int width, int height, int stride);
    static Bitmap ConvertToGrayscale(const Bitmap& bitmap);
    static std::string InfoSave(const Bitmap& bitmap);
    static std::string DebugSave(const Bitmap& bitmap);
    static std::string DebugSave(const Bitmap& bitmap, const std::string& filename);
    static Bitmap CopyBitmap(const Bitmap& bitmap, const std::optional<Rectangle>& rectangle, Point& top_left);
    static Bitmap GetBitmap(const Bitmap& bitmap, Rectangle rectangle);
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_BITMAPHELPER_HPP
