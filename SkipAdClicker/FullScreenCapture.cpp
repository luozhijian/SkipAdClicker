#include "FullScreenCapture.hpp"

#include "../Utilities/Logger.hpp"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <QGuiApplication>
#include <QImage>
#include <QPixmap>
#include <QRect>
#include <QScreen>
#if defined(__linux__)
#if defined(AUTOMATIOTEST_HAS_QT_DBUS)
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QEventLoop>
#include <QObject>
#include <QTimer>
#include <QUrl>
#include <QVariant>
#include <QVariantMap>
#endif
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif
#endif

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace automationtest::app {

using automationtest::utilities::Logger;

#if defined(__linux__) && defined(AUTOMATIOTEST_HAS_QT_DBUS)
namespace full_screen_capture_detail {

class PortalResponseReceiver : public QObject {
    Q_OBJECT

public:
    explicit PortalResponseReceiver(QEventLoop& loop)
        : loop_(&loop)
    {
    }

    [[nodiscard]] bool Completed() const noexcept
    {
        return completed_;
    }

    [[nodiscard]] uint ResponseCode() const noexcept
    {
        return response_code_;
    }

    [[nodiscard]] const QVariantMap& Results() const noexcept
    {
        return results_;
    }

public slots:
    void HandleResponse(uint response_code, const QVariantMap& results)
    {
        completed_ = true;
        response_code_ = response_code;
        results_ = results;
        if (loop_ != nullptr) {
            loop_->quit();
        }
    }

private:
    QEventLoop* loop_ {nullptr};
    bool completed_ {false};
    uint response_code_ {1};
    QVariantMap results_ {};
};

} // namespace full_screen_capture_detail
#endif

namespace {

constexpr const char* LogCategory = "FullScreenCapture";

bool IsAllBlackCapture(const utilities::Bitmap& bitmap)
{
    if (bitmap.width <= 0 || bitmap.height <= 0 || bitmap.stride <= 0 || bitmap.pixels.empty()) {
        return true;
    }

    const int channels = bitmap.channels > 0 ? bitmap.channels : 1;
    const auto required_size = static_cast<std::size_t>(bitmap.stride) * static_cast<std::size_t>(bitmap.height);
    if (bitmap.pixels.size() < required_size) {
        return true;
    }

    for (int y = 0; y < bitmap.height; ++y) {
        const auto row_start = static_cast<std::size_t>(y) * static_cast<std::size_t>(bitmap.stride);
        for (int x = 0; x < bitmap.width; ++x) {
            const auto pixel_start = row_start + static_cast<std::size_t>(x) * static_cast<std::size_t>(channels);
            if (pixel_start >= bitmap.pixels.size()) {
                return true;
            }

            const auto blue_or_gray = static_cast<unsigned char>(bitmap.pixels[pixel_start]);
            const auto green = channels > 1 && pixel_start + 1 < bitmap.pixels.size()
                ? static_cast<unsigned char>(bitmap.pixels[pixel_start + 1])
                : blue_or_gray;
            const auto red = channels > 2 && pixel_start + 2 < bitmap.pixels.size()
                ? static_cast<unsigned char>(bitmap.pixels[pixel_start + 2])
                : blue_or_gray;
            const auto alpha = channels > 3 && pixel_start + 3 < bitmap.pixels.size()
                ? static_cast<unsigned char>(bitmap.pixels[pixel_start + 3])
                : 255;

            if (blue_or_gray != 0 || green != 0 || red != 0 || alpha != 255) {
                return false;
            }
        }
    }

    return true;
}

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
    std::shared_ptr<FullScreenCapture::ScreenshotList> screenshots {
        std::make_shared<FullScreenCapture::ScreenshotList>()};
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

#if defined(__linux__)
int last_x11_error_code = 0;

int TrapX11Error(Display*, XErrorEvent* event)
{
    last_x11_error_code = event != nullptr ? event->error_code : -1;
    return 0;
}

int MaskShift(unsigned long mask) noexcept
{
    int shift = 0;
    while (mask != 0 && (mask & 1UL) == 0) {
        mask >>= 1;
        ++shift;
    }
    return shift;
}

int MaskBits(unsigned long mask) noexcept
{
    int bits = 0;
    while (mask != 0) {
        if ((mask & 1UL) != 0) {
            ++bits;
        }
        mask >>= 1;
    }
    return bits;
}

std::byte ScalePixelChannel(unsigned long pixel, unsigned long mask, int shift, int bits) noexcept
{
    if (mask == 0 || bits <= 0) {
        return std::byte {0};
    }

    const unsigned long value = (pixel & mask) >> shift;
    const unsigned long maximum = (1UL << static_cast<unsigned long>(bits)) - 1UL;
    return static_cast<std::byte>((value * 255UL + maximum / 2UL) / maximum);
}

bool CopyXImageToBitmap(XImage* image, utilities::Bitmap& bitmap)
{
    if (image == nullptr || image->width <= 0 || image->height <= 0) {
        return false;
    }

    const int red_shift = MaskShift(image->red_mask);
    const int green_shift = MaskShift(image->green_mask);
    const int blue_shift = MaskShift(image->blue_mask);
    const int red_bits = MaskBits(image->red_mask);
    const int green_bits = MaskBits(image->green_mask);
    const int blue_bits = MaskBits(image->blue_mask);

    bitmap.width = image->width;
    bitmap.height = image->height;
    bitmap.channels = 4;
    bitmap.stride = bitmap.width * bitmap.channels;

    const auto byte_count = static_cast<std::size_t>(bitmap.stride) * static_cast<std::size_t>(bitmap.height);
    bitmap.pixels.assign(byte_count, std::byte {0});

    for (int y = 0; y < bitmap.height; ++y) {
        const auto row_start = static_cast<std::size_t>(y) * static_cast<std::size_t>(bitmap.stride);
        for (int x = 0; x < bitmap.width; ++x) {
            const unsigned long pixel = XGetPixel(image, x, y);
            const auto target = row_start + static_cast<std::size_t>(x) * static_cast<std::size_t>(bitmap.channels);
            bitmap.pixels[target] = ScalePixelChannel(pixel, image->blue_mask, blue_shift, blue_bits);
            bitmap.pixels[target + 1] = ScalePixelChannel(pixel, image->green_mask, green_shift, green_bits);
            bitmap.pixels[target + 2] = ScalePixelChannel(pixel, image->red_mask, red_shift, red_bits);
            bitmap.pixels[target + 3] = std::byte {255};
        }
    }

    return true;
}

bool CaptureRootWindowWithX11(FullScreenCapture::ScreenshotList& screenshots)
{
    Display* display = XOpenDisplay(nullptr);
    if (display == nullptr) {
        Logger::Error("X11 fallback screenshot failed because DISPLAY could not be opened.", LogCategory);
        return false;
    }

    const int screen = DefaultScreen(display);
    const Window root = RootWindow(display, screen);
    XWindowAttributes attributes {};
    if (XGetWindowAttributes(display, root, &attributes) == 0 || attributes.width <= 0 || attributes.height <= 0) {
        Logger::Error("X11 fallback screenshot failed because the root window size could not be read.", LogCategory);
        XCloseDisplay(display);
        return false;
    }

    last_x11_error_code = 0;
    const auto previous_error_handler = XSetErrorHandler(TrapX11Error);
    XImage* image =
        XGetImage(
            display,
            root,
            0,
            0,
            static_cast<unsigned int>(attributes.width),
            static_cast<unsigned int>(attributes.height),
            AllPlanes,
            ZPixmap);
    XSync(display, False);
    XSetErrorHandler(previous_error_handler);

    if (last_x11_error_code != 0 || image == nullptr) {
        if (image != nullptr) {
            XDestroyImage(image);
        }
        Logger::Error(
            "X11 fallback screenshot failed. XGetImage error code="
                + std::to_string(last_x11_error_code)
                + ". Ubuntu Wayland/XWayland commonly blocks root-window screenshots.",
            LogCategory);
        XCloseDisplay(display);
        return false;
    }

    auto& [bitmap, location] = screenshots.emplace_back();
    const bool copied = CopyXImageToBitmap(image, bitmap);
    XDestroyImage(image);
    XCloseDisplay(display);

    if (!copied) {
        screenshots.pop_back();
        Logger::Error("X11 fallback screenshot failed while copying pixels.", LogCategory);
        return false;
    }
    if (IsAllBlackCapture(bitmap)) {
        screenshots.pop_back();
        Logger::Error(
            "X11 fallback screenshot returned an all-black image. Ubuntu Wayland/XWayland is not exposing the real desktop pixels.",
            LogCategory);
        return false;
    }

    location = utilities::Point {0, 0};
    return true;
}
#endif

std::shared_ptr<FullScreenCapture::ScreenshotList>& PortableScreenshots()
{
    static auto screenshots = std::make_shared<FullScreenCapture::ScreenshotList>();
    return screenshots;
}

bool CopyQImageToBitmap(const QImage& image, utilities::Bitmap& bitmap)
{
    if (image.isNull() || image.width() <= 0 || image.height() <= 0 || image.sizeInBytes() <= 0) {
        return false;
    }

    bitmap.width = image.width();
    bitmap.height = image.height();
    bitmap.channels = 4;
    bitmap.stride = image.bytesPerLine();

    const auto byte_count = static_cast<std::size_t>(image.sizeInBytes());
    if (bitmap.pixels.size() != byte_count) {
        bitmap.pixels.resize(byte_count);
    }
    std::memcpy(bitmap.pixels.data(), image.constBits(), byte_count);
    return true;
}

bool AddQImageScreenshot(
    FullScreenCapture::ScreenshotList& screenshots,
    const QImage& source_image,
    const utilities::Point& location,
    const std::string& source_name)
{
    const auto image = source_image.convertToFormat(QImage::Format_ARGB32);
    auto& [bitmap, bitmap_location] = screenshots.emplace_back();
    if (!CopyQImageToBitmap(image, bitmap)) {
        screenshots.pop_back();
        Logger::Error(source_name + " screenshot failed while copying pixels.", LogCategory);
        return false;
    }
    if (IsAllBlackCapture(bitmap)) {
        screenshots.pop_back();
        Logger::Error(source_name + " screenshot returned an all-black image.", LogCategory);
        return false;
    }

    bitmap_location = location;
    return true;
}

utilities::Point VirtualDesktopOrigin()
{
    const auto screens = QGuiApplication::screens();
    if (screens.empty() || screens.front() == nullptr) {
        return utilities::Point {0, 0};
    }

    QRect bounds = screens.front()->geometry();
    for (auto* screen : screens) {
        if (screen != nullptr) {
            bounds = bounds.united(screen->geometry());
        }
    }

    return utilities::Point {bounds.x(), bounds.y()};
}

void CaptureScreenshotsWithQt(FullScreenCapture::ScreenshotList& screenshots)
{
    const auto screens = QGuiApplication::screens();
    screenshots.clear();
    screenshots.reserve(static_cast<std::size_t>(screens.size()));

    for (int index = 0; index < screens.size(); ++index) {
        auto* screen = screens[index];
        if (screen == nullptr) {
            Logger::Error("Screen capture skipped because QGuiApplication returned a null screen.", LogCategory);
            continue;
        }

        const auto geometry = screen->geometry();
        const auto image = screen->grabWindow(0).toImage();
        if (image.isNull() || image.width() <= 0 || image.height() <= 0 || image.sizeInBytes() <= 0) {
            Logger::Error(
                "Screen capture failed for screen " + std::to_string(index)
                    + " using Qt platform '" + QGuiApplication::platformName().toStdString()
                    + "'. On Wayland, global screenshots may be blocked; use an Xorg session or a portal-based capture path.",
                LogCategory);
            continue;
        }

        AddQImageScreenshot(
            screenshots,
            image,
            utilities::Point {geometry.x(), geometry.y()},
            "Qt screen " + std::to_string(index));
    }
}

#if defined(__linux__)
bool IsWaylandSession()
{
    const char* session_type = std::getenv("XDG_SESSION_TYPE");
    if (session_type != nullptr && std::string(session_type) == "wayland") {
        return true;
    }

    const char* wayland_display = std::getenv("WAYLAND_DISPLAY");
    if (wayland_display != nullptr && wayland_display[0] != '\0') {
        return true;
    }

    return QGuiApplication::platformName().toLower().contains("wayland");
}

#if defined(AUTOMATIOTEST_HAS_QT_DBUS)
bool WaitForPortalResponse(QDBusConnection& bus, const QString& handle_path, QVariantMap& results)
{
    QEventLoop loop;
    full_screen_capture_detail::PortalResponseReceiver receiver(loop);
    const bool connected = bus.connect(
        QStringLiteral("org.freedesktop.portal.Desktop"),
        handle_path,
        QStringLiteral("org.freedesktop.portal.Request"),
        QStringLiteral("Response"),
        &receiver,
        SLOT(HandleResponse(uint,QVariantMap)));
    if (!connected) {
        Logger::Error("Wayland portal screenshot failed because the portal response signal could not be connected.", LogCategory);
        return false;
    }

    QTimer timeout;
    timeout.setSingleShot(true);
    QObject::connect(&timeout, &QTimer::timeout, &loop, &QEventLoop::quit);
    timeout.start(30000);
    loop.exec();

    bus.disconnect(
        QStringLiteral("org.freedesktop.portal.Desktop"),
        handle_path,
        QStringLiteral("org.freedesktop.portal.Request"),
        QStringLiteral("Response"),
        &receiver,
        SLOT(HandleResponse(uint,QVariantMap)));

    if (!receiver.Completed()) {
        Logger::Error("Wayland portal screenshot timed out waiting for user approval.", LogCategory);
        return false;
    }

    if (receiver.ResponseCode() != 0) {
        Logger::Error(
            "Wayland portal screenshot was not approved. Portal response code="
                + std::to_string(receiver.ResponseCode()) + ".",
            LogCategory);
        return false;
    }

    results = receiver.Results();
    return true;
}

bool CaptureWaylandScreenshotWithPortal(FullScreenCapture::ScreenshotList& screenshots)
{
    screenshots.clear();

    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.isConnected()) {
        Logger::Error("Wayland portal screenshot failed because the session DBus is not connected.", LogCategory);
        return false;
    }

    QDBusInterface portal(
        QStringLiteral("org.freedesktop.portal.Desktop"),
        QStringLiteral("/org/freedesktop/portal/desktop"),
        QStringLiteral("org.freedesktop.portal.Screenshot"),
        bus);
    if (!portal.isValid()) {
        Logger::Error(
            "Wayland portal screenshot failed because org.freedesktop.portal.Screenshot is not available: "
                + portal.lastError().message().toStdString(),
            LogCategory);
        return false;
    }

    QVariantMap options;
    options.insert(QStringLiteral("interactive"), false);
    options.insert(QStringLiteral("modal"), true);

    const QDBusMessage reply = portal.call(QStringLiteral("Screenshot"), QString(), options);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        Logger::Error(
            "Wayland portal screenshot request failed: " + reply.errorMessage().toStdString(),
            LogCategory);
        return false;
    }

    const auto arguments = reply.arguments();
    if (arguments.isEmpty() || !arguments.first().canConvert<QDBusObjectPath>()) {
        Logger::Error("Wayland portal screenshot failed because the portal did not return a request handle.", LogCategory);
        return false;
    }

    const auto handle = arguments.first().value<QDBusObjectPath>();
    if (handle.path().isEmpty()) {
        Logger::Error("Wayland portal screenshot failed because the portal returned an empty request handle.", LogCategory);
        return false;
    }

    Logger::Info("Requesting a Wayland screenshot through xdg-desktop-portal.", LogCategory);

    QVariantMap results;
    if (!WaitForPortalResponse(bus, handle.path(), results)) {
        return false;
    }

    const QString uri = results.value(QStringLiteral("uri")).toString();
    if (uri.isEmpty()) {
        Logger::Error("Wayland portal screenshot succeeded but did not return an image URI.", LogCategory);
        return false;
    }

    const QString image_path = QUrl(uri).toLocalFile();
    if (image_path.isEmpty()) {
        Logger::Error("Wayland portal screenshot returned a non-local image URI: " + uri.toStdString(), LogCategory);
        return false;
    }

    const QImage image(image_path);
    if (!AddQImageScreenshot(screenshots, image, VirtualDesktopOrigin(), "Wayland portal")) {
        return false;
    }

    Logger::Info("Wayland portal screenshot captured " + std::to_string(screenshots.front().first.width)
            + "x" + std::to_string(screenshots.front().first.height) + " pixels.",
        LogCategory);
    return true;
}
#else
bool CaptureWaylandScreenshotWithPortal(FullScreenCapture::ScreenshotList& screenshots)
{
    screenshots.clear();
    Logger::Error("Wayland portal screenshot support is not built because Qt DBus was not found.", LogCategory);
    return false;
}
#endif

void CaptureLinuxFallbackIfNeeded(FullScreenCapture::ScreenshotList& screenshots)
{
    if (!screenshots.empty()) {
        return;
    }

    Logger::Info("Qt screen capture returned no images; trying X11 root-window fallback.", LogCategory);
    CaptureRootWindowWithX11(screenshots);
}
#endif

void LogEmptyCaptureResult(const FullScreenCapture::ScreenshotList& screenshots)
{
    if (!screenshots.empty()) {
        return;
    }

    Logger::Error("TakeFullScreenshot did not capture any images.", LogCategory);
#if defined(__linux__)
    if (IsWaylandSession()) {
        Logger::Error(
            "This process is running in a Wayland session. Wayland requires a user-approved portal path; unattended repeated screenshots require the ScreenCast/PipeWire portal.",
            LogCategory);
    }
#endif
}

#endif

} // namespace

std::shared_ptr<FullScreenCapture::ScreenshotList> FullScreenCapture::TakeFullScreenshot()
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

#if defined(__linux__)
    if (IsWaylandSession()) {
        CaptureWaylandScreenshotWithPortal(*screenshots);
        LogEmptyCaptureResult(*screenshots);
        return screenshots;
    }
#endif

    CaptureScreenshotsWithQt(*screenshots);

#if defined(__linux__)
    CaptureLinuxFallbackIfNeeded(*screenshots);
#endif

    LogEmptyCaptureResult(*screenshots);

    return screenshots;
#endif
}

} // namespace automationtest::app

#if defined(__linux__) && defined(AUTOMATIOTEST_HAS_QT_DBUS)
#include "FullScreenCapture.moc"
#endif
