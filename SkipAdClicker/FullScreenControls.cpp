#include "FullScreenControls.hpp"

#include <QGuiApplication>
#include <QImage>
#include <QPixmap>
#include <QScreen>

#include <cstring>

namespace automationtest::app {

std::vector<std::pair<utilities::Bitmap, utilities::Point>> FullScreenControls::TakeFullScreenshot()
{
    std::vector<std::pair<utilities::Bitmap, utilities::Point>> result;
    for (auto* screen : QGuiApplication::screens()) {
        const auto geometry = screen->geometry();
        const auto image = screen->grabWindow(0).toImage().convertToFormat(QImage::Format_RGBA8888);

        utilities::Bitmap bitmap;
        bitmap.width = image.width();
        bitmap.height = image.height();
        bitmap.channels = 4;
        bitmap.stride = image.bytesPerLine();
        bitmap.pixels.resize(static_cast<std::size_t>(image.sizeInBytes()));
        std::memcpy(bitmap.pixels.data(), image.constBits(), static_cast<std::size_t>(image.sizeInBytes()));

        result.emplace_back(std::move(bitmap), utilities::Point {geometry.x(), geometry.y()});
    }
    return result;
}

void FullScreenControls::RegisterBindings(utilities::status::LoadFunctions& load_functions)
{
    load_functions.RegisterMethod("TakeFullScreenshot", utilities::status::LoadFunctions::RegisteredMethod {
        .declaring_type = "FullScreenControls",
        .invoke = [](const std::vector<std::any>&) -> std::any {
            return TakeFullScreenshot();
        },
    });
}

} // namespace automationtest::app
