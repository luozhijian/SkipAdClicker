#ifndef AUTOMATIOTEST_WINDOWSAPI_USER32LIB_HPP
#define AUTOMATIOTEST_WINDOWSAPI_USER32LIB_HPP

#include "../CommonTypes.hpp"

#include <cstdint>
#include <string>

namespace automationtest::utilities {

class User32Lib {
public:
    using NativeWindowHandle = void*;
    using NativeMonitorHandle = void*;

    static constexpr int DpiAwarenessContextPerMonitorAwareV2 = -4;

    [[nodiscard]] static NativeWindowHandle GetForegroundWindowHandle();
    [[nodiscard]] static bool SetProcessDpiAwarenessContext(int dpi_context);
    [[nodiscard]] static NativeMonitorHandle MonitorFromPoint(Point point);
    [[nodiscard]] static Rectangle GetScreenSize();
    [[nodiscard]] static Bitmap CaptureWindowUseBitBlt(NativeWindowHandle window_handle, const std::string& display_name = {});
    [[nodiscard]] static Bitmap CaptureWindowUsePrintWindow(NativeWindowHandle window_handle);
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_WINDOWSAPI_USER32LIB_HPP
