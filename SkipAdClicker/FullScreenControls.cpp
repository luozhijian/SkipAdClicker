#include "FullScreenControls.hpp"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <QGuiApplication>
#include <QImage>
#include <QScreen>
#endif

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <memory>
#include <vector>

namespace automationtest::app {

namespace {

#ifdef _WIN32

struct MonitorCapture {
    HDC memory_dc {nullptr};
    HBITMAP dib {nullptr};
    HGDIOBJ old_object {nullptr};
    void* bits {nullptr};
    int width {0};
    int height {0};

    MonitorCapture() = default;
    MonitorCapture(const MonitorCapture&) = delete;
    MonitorCapture& operator=(const MonitorCapture&) = delete;

    MonitorCapture(MonitorCapture&& other) noexcept
    {
        *this = std::move(other);
    }

    MonitorCapture& operator=(MonitorCapture&& other) noexcept
    {
        if (this != &other) {
            Release();
            memory_dc = other.memory_dc;
            dib = other.dib;
            old_object = other.old_object;
            bits = other.bits;
            width = other.width;
            height = other.height;

            other.memory_dc = nullptr;
            other.dib = nullptr;
            other.old_object = nullptr;
            other.bits = nullptr;
            other.width = 0;
            other.height = 0;
        }
        return *this;
    }

    ~MonitorCapture()
    {
        Release();
    }

    void Release()
    {
        if (memory_dc != nullptr && old_object != nullptr) {
            SelectObject(memory_dc, old_object);
        }
        if (dib != nullptr) {
            DeleteObject(dib);
        }
        if (memory_dc != nullptr) {
            DeleteDC(memory_dc);
        }

        memory_dc = nullptr;
        dib = nullptr;
        old_object = nullptr;
        bits = nullptr;
        width = 0;
        height = 0;
    }

    bool Ensure(HDC screen_dc, int requested_width, int requested_height)
    {
        if (requested_width <= 0 || requested_height <= 0) {
            Release();
            return false;
        }

        if (memory_dc != nullptr && dib != nullptr && width == requested_width && height == requested_height) {
            return true;
        }

        Release();

        BITMAPINFO info {};
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = requested_width;
        info.bmiHeader.biHeight = -requested_height;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;

        void* dib_bits = nullptr;
        HBITMAP new_dib = CreateDIBSection(screen_dc, &info, DIB_RGB_COLORS, &dib_bits, nullptr, 0);
        if (new_dib == nullptr || dib_bits == nullptr) {
            if (new_dib != nullptr) {
                DeleteObject(new_dib);
            }
            return false;
        }

        HDC new_memory_dc = CreateCompatibleDC(screen_dc);
        if (new_memory_dc == nullptr) {
            DeleteObject(new_dib);
            return false;
        }

        HGDIOBJ previous = SelectObject(new_memory_dc, new_dib);
        if (previous == nullptr || previous == HGDI_ERROR) {
            DeleteDC(new_memory_dc);
            DeleteObject(new_dib);
            return false;
        }

        memory_dc = new_memory_dc;
        dib = new_dib;
        old_object = previous;
        bits = dib_bits;
        width = requested_width;
        height = requested_height;
        return true;
    }
};

struct CaptureState {
    std::shared_ptr<FullScreenControls::ScreenshotList> screenshots {
        std::make_shared<FullScreenControls::ScreenshotList>()};
    std::vector<MonitorCapture> captures {};
    std::size_t monitor_index {0};
};

CaptureState& State()
{
    static CaptureState state {};
    return state;
}

BOOL CALLBACK CaptureMonitor(HMONITOR, HDC, LPRECT rect, LPARAM data)
{
    auto& state = *reinterpret_cast<CaptureState*>(data);
    const int width = rect->right - rect->left;
    const int height = rect->bottom - rect->top;
    if (width <= 0 || height <= 0) {
        return TRUE;
    }

    HDC screen_dc = GetDC(nullptr);
    if (screen_dc == nullptr) {
        return TRUE;
    }

    const auto index = state.monitor_index++;
    if (state.captures.size() <= index) {
        state.captures.emplace_back();
    }

    auto& capture = state.captures[index];
    if (!capture.Ensure(screen_dc, width, height)) {
        ReleaseDC(nullptr, screen_dc);
        return TRUE;
    }

    const BOOL copied = BitBlt(
        capture.memory_dc,
        0,
        0,
        width,
        height,
        screen_dc,
        rect->left,
        rect->top,
        SRCCOPY | CAPTUREBLT);
    ReleaseDC(nullptr, screen_dc);
    if (!copied) {
        return TRUE;
    }

    if (state.screenshots->size() <= index) {
        state.screenshots->emplace_back();
    }

    auto& [bitmap, location] = (*state.screenshots)[index];
    bitmap.width = width;
    bitmap.height = height;
    bitmap.channels = 4;
    bitmap.stride = width * bitmap.channels;
    const auto byte_count = static_cast<std::size_t>(bitmap.stride) * static_cast<std::size_t>(height);
    if (bitmap.pixels.size() != byte_count) {
        bitmap.pixels.resize(byte_count);
    }
    std::memcpy(bitmap.pixels.data(), capture.bits, byte_count);
    location = utilities::Point {rect->left, rect->top};

    return TRUE;
}

#else

std::shared_ptr<FullScreenControls::ScreenshotList>& PortableScreenshots()
{
    static auto screenshots = std::make_shared<FullScreenControls::ScreenshotList>();
    return screenshots;
}

#endif

} // namespace

std::shared_ptr<FullScreenControls::ScreenshotList> FullScreenControls::TakeFullScreenshot()
{
#ifdef _WIN32
    auto& state = State();
    state.monitor_index = 0;
    EnumDisplayMonitors(nullptr, nullptr, CaptureMonitor, reinterpret_cast<LPARAM>(&state));

    if (state.screenshots->size() > state.monitor_index) {
        state.screenshots->resize(state.monitor_index);
    }
    if (state.captures.size() > state.monitor_index) {
        state.captures.resize(state.monitor_index);
    }

    return state.screenshots;
#else
    auto& screenshots = PortableScreenshots();
    const auto screens = QGuiApplication::screens();
    screenshots->resize(static_cast<std::size_t>(screens.size()));

    for (int index = 0; index < screens.size(); ++index) {
        const auto* screen = screens[index];
        const auto geometry = screen->geometry();
        const auto image = screen->grabWindow(0).toImage().convertToFormat(QImage::Format_RGBA8888);

        auto& [bitmap, location] = (*screenshots)[static_cast<std::size_t>(index)];
        bitmap.width = image.width();
        bitmap.height = image.height();
        bitmap.channels = 4;
        bitmap.stride = image.bytesPerLine();

        const auto byte_count = static_cast<std::size_t>(image.sizeInBytes());
        if (bitmap.pixels.size() != byte_count) {
            bitmap.pixels.resize(byte_count);
        }
        std::memcpy(bitmap.pixels.data(), image.constBits(), byte_count);
        location = utilities::Point {geometry.x(), geometry.y()};
    }

    return screenshots;
#endif
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
