#include "User32Lib.hpp"

#include "../BitmapHelper.hpp"

#include <stdexcept>
#include <string>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace automationtest::utilities {

#ifdef _WIN32
namespace {

constexpr int Srccopy = 0x00CC0020;
constexpr unsigned int MonitorDefaultToNearest = 2;
constexpr int EnumCurrentSettings = -1;

HWND ToHwnd(User32Lib::NativeWindowHandle handle)
{
    return static_cast<HWND>(handle);
}

Bitmap CopyBgraPixelsToBitmap(const void* source_pixels, int width, int height)
{
    Bitmap bitmap;
    bitmap.width = width;
    bitmap.height = height;
    bitmap.channels = 3;
    bitmap.stride = width * bitmap.channels;
    bitmap.pixels.resize(static_cast<std::size_t>(bitmap.stride * bitmap.height));

    const auto* bgra = static_cast<const unsigned char*>(source_pixels);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const auto source_index = static_cast<std::size_t>((y * width + x) * 4);
            const auto target_index = static_cast<std::size_t>(y * bitmap.stride + x * bitmap.channels);
            bitmap.pixels[target_index] = static_cast<std::byte>(bgra[source_index + 2]);
            bitmap.pixels[target_index + 1] = static_cast<std::byte>(bgra[source_index + 1]);
            bitmap.pixels[target_index + 2] = static_cast<std::byte>(bgra[source_index]);
        }
    }
    return bitmap;
}

Bitmap CaptureWindow(User32Lib::NativeWindowHandle window_handle, const std::string& display_name, bool use_print_window)
{
    const HWND hwnd = ToHwnd(window_handle);
    if (hwnd == nullptr) {
        throw std::invalid_argument("Invalid window handle.");
    }

    RECT rect {};
    if (!GetWindowRect(hwnd, &rect)) {
        throw std::runtime_error("Failed to get window rectangle.");
    }

    const int width = rect.right - rect.left;
    const int height = rect.bottom - rect.top;
    if (width <= 0 || height <= 0) {
        throw std::runtime_error("Window rectangle is empty.");
    }

    HDC window_dc = GetWindowDC(hwnd);
    if (window_dc == nullptr) {
        throw std::runtime_error("Failed to get window DC.");
    }

    HDC memory_dc = nullptr;
    HBITMAP dib = nullptr;
    HGDIOBJ previous = nullptr;
    void* pixels = nullptr;

    try {
        memory_dc = CreateCompatibleDC(window_dc);
        if (memory_dc == nullptr) {
            throw std::runtime_error("Failed to create compatible DC.");
        }

        BITMAPINFO bitmap_info {};
        bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bitmap_info.bmiHeader.biWidth = width;
        bitmap_info.bmiHeader.biHeight = -height;
        bitmap_info.bmiHeader.biPlanes = 1;
        bitmap_info.bmiHeader.biBitCount = 32;
        bitmap_info.bmiHeader.biCompression = BI_RGB;

        dib = CreateDIBSection(window_dc, &bitmap_info, DIB_RGB_COLORS, &pixels, nullptr, 0);
        if (dib == nullptr || pixels == nullptr) {
            throw std::runtime_error("Failed to create capture bitmap.");
        }

        previous = SelectObject(memory_dc, dib);
        if (previous == nullptr || previous == HGDI_ERROR) {
            throw std::runtime_error("Failed to select capture bitmap.");
        }

        const BOOL captured = use_print_window
            ? PrintWindow(hwnd, memory_dc, 0)
            : BitBlt(memory_dc, 0, 0, width, height, window_dc, 0, 0, Srccopy);
        if (!captured) {
            throw std::runtime_error("Failed to capture the window content.");
        }

        auto bitmap = CopyBgraPixelsToBitmap(pixels, width, height);
        BitmapHelper::DebugSave(bitmap, "c:\\temp\\luo\\screenshot.bmp");

        SelectObject(memory_dc, previous);
        DeleteObject(dib);
        DeleteDC(memory_dc);
        ReleaseDC(hwnd, window_dc);
        return bitmap;
    } catch (const std::exception& exception) {
        if (previous != nullptr && previous != HGDI_ERROR && memory_dc != nullptr) {
            SelectObject(memory_dc, previous);
        }
        if (dib != nullptr) {
            DeleteObject(dib);
        }
        if (memory_dc != nullptr) {
            DeleteDC(memory_dc);
        }
        ReleaseDC(hwnd, window_dc);

        if (!display_name.empty()) {
            throw std::runtime_error("Failed to get screenshot " + display_name + ": " + exception.what());
        }
        throw;
    }
}

} // namespace
#endif

User32Lib::NativeWindowHandle User32Lib::GetForegroundWindowHandle()
{
#ifdef _WIN32
    return GetForegroundWindow();
#else
    return nullptr;
#endif
}

bool User32Lib::SetProcessDpiAwarenessContext(int dpi_context)
{
#ifdef _WIN32
    const auto context = reinterpret_cast<DPI_AWARENESS_CONTEXT>(static_cast<intptr_t>(dpi_context));
    return ::SetProcessDpiAwarenessContext(context) != FALSE;
#else
    (void)dpi_context;
    return false;
#endif
}

User32Lib::NativeMonitorHandle User32Lib::MonitorFromPoint(Point point)
{
#ifdef _WIN32
    POINT native_point {point.x, point.y};
    return ::MonitorFromPoint(native_point, MonitorDefaultToNearest);
#else
    (void)point;
    return nullptr;
#endif
}

Rectangle User32Lib::GetScreenSize()
{
#ifdef _WIN32
    DEVMODEA dev_mode {};
    dev_mode.dmSize = sizeof(DEVMODEA);
    if (!EnumDisplaySettingsA(nullptr, EnumCurrentSettings, &dev_mode)) {
        throw std::runtime_error("Failed to enumerate display settings.");
    }
    return Rectangle {0, 0, static_cast<int>(dev_mode.dmPelsWidth), static_cast<int>(dev_mode.dmPelsHeight)};
#else
    return Rectangle {};
#endif
}

Bitmap User32Lib::CaptureWindowUseBitBlt(NativeWindowHandle window_handle, const std::string& display_name)
{
#ifdef _WIN32
    return CaptureWindow(window_handle, display_name, false);
#else
    (void)window_handle;
    (void)display_name;
    throw std::runtime_error("CaptureWindowUseBitBlt is only available on Windows.");
#endif
}

Bitmap User32Lib::CaptureWindowUsePrintWindow(NativeWindowHandle window_handle)
{
#ifdef _WIN32
    return CaptureWindow(window_handle, {}, true);
#else
    (void)window_handle;
    throw std::runtime_error("CaptureWindowUsePrintWindow is only available on Windows.");
#endif
}

} // namespace automationtest::utilities
