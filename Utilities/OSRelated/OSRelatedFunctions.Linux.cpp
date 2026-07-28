#include "OSRelatedFunctions.hpp"

#if defined(__linux__)

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#if defined(AUTOMATIOTEST_HAS_XINERAMA)
#include <X11/extensions/Xinerama.h>
#endif

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

namespace {

struct DisplayCloser
{
    void operator()(Display* display) const
    {
        if (display != nullptr) {
            XCloseDisplay(display);
        }
    }
};

using DisplayPointer = std::unique_ptr<Display, DisplayCloser>;

int IgnoreXError(Display*, XErrorEvent*)
{
    return 0;
}

class XErrorHandlerGuard {
public:
    explicit XErrorHandlerGuard(Display* display)
        : display_(display)
        , previous_handler_(XSetErrorHandler(IgnoreXError))
    {
    }

    ~XErrorHandlerGuard()
    {
        XSync(display_, False);
        XSetErrorHandler(previous_handler_);
    }

private:
    Display* display_ {};
    XErrorHandler previous_handler_ {};
};

std::vector<unsigned long> ReadCardinalList(Display* display, Window window, Atom property)
{
    if (property == None) {
        return {};
    }
    Atom actual_type {};
    int actual_format {};
    unsigned long item_count {};
    unsigned long bytes_after {};
    unsigned char* data {};
    if (XGetWindowProperty(
            display,
            window,
            property,
            0,
            4096,
            False,
            AnyPropertyType,
            &actual_type,
            &actual_format,
            &item_count,
            &bytes_after,
            &data)
        != Success
        || data == nullptr
        || actual_format != 32) {
        if (data != nullptr) {
            XFree(data);
        }
        return {};
    }

    const auto* values = reinterpret_cast<unsigned long*>(data);
    std::vector<unsigned long> result(values, values + item_count);
    XFree(data);
    return result;
}

std::vector<std::string> ReadStringProperty(Display* display, Window window, Atom property)
{
    if (property == None) {
        return {};
    }
    Atom actual_type {};
    int actual_format {};
    unsigned long item_count {};
    unsigned long bytes_after {};
    unsigned char* data {};
    if (XGetWindowProperty(
            display,
            window,
            property,
            0,
            4096,
            False,
            AnyPropertyType,
            &actual_type,
            &actual_format,
            &item_count,
            &bytes_after,
            &data)
        != Success
        || data == nullptr
        || actual_format != 8) {
        if (data != nullptr) {
            XFree(data);
        }
        return {};
    }

    std::vector<std::string> result;
    const char* begin = reinterpret_cast<const char*>(data);
    const char* end = begin + item_count;
    while (begin < end) {
        const auto* terminator = std::find(begin, end, '\0');
        if (terminator != begin) {
            result.emplace_back(begin, terminator);
        }
        begin = terminator == end ? end : terminator + 1;
    }
    XFree(data);
    return result;
}

std::optional<unsigned long> ReadFirstCardinal(Display* display, Window window, Atom property)
{
    const auto values = ReadCardinalList(display, window, property);
    if (values.empty()) {
        return std::nullopt;
    }
    return values.front();
}

std::string Lowercase(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char character) {
        return static_cast<char>(std::tolower(character));
    });
    return value;
}

std::string ProcessExecutableName(unsigned long process_id)
{
    std::error_code error;
    const auto executable = std::filesystem::read_symlink(
        std::filesystem::path("/proc") / std::to_string(process_id) / "exe",
        error);
    return error ? std::string {} : executable.filename().string();
}

bool ContainsBrowserIdentifier(
    Display* display,
    Window window,
    const std::unordered_set<std::string>& browsers,
    Atom process_id_property,
    Atom application_id_property)
{
    std::vector<std::string> identifiers;
    if (const auto process_id = ReadFirstCardinal(display, window, process_id_property); process_id.has_value()) {
        identifiers.push_back(ProcessExecutableName(*process_id));
    }

    const auto application_ids = ReadStringProperty(display, window, application_id_property);
    identifiers.insert(identifiers.end(), application_ids.begin(), application_ids.end());

    XClassHint class_hint {};
    if (XGetClassHint(display, window, &class_hint) != 0) {
        if (class_hint.res_name != nullptr) {
            identifiers.emplace_back(class_hint.res_name);
            XFree(class_hint.res_name);
        }
        if (class_hint.res_class != nullptr) {
            identifiers.emplace_back(class_hint.res_class);
            XFree(class_hint.res_class);
        }
    }

    return std::any_of(identifiers.begin(), identifiers.end(), [&](const auto& identifier) {
        const auto normalized_identifier = Lowercase(identifier);
        return browsers.contains(normalized_identifier);
    });
}

bool IsOnCurrentDesktop(Display* display, Window window, Atom window_desktop_property, Atom current_desktop_property)
{
    const auto window_desktop = ReadFirstCardinal(display, window, window_desktop_property);
    const auto current_desktop = ReadFirstCardinal(display, DefaultRootWindow(display), current_desktop_property);
    constexpr unsigned long all_desktops = 0xFFFFFFFFUL;
    return !window_desktop.has_value()
        || !current_desktop.has_value()
        || *window_desktop == all_desktops
        || *window_desktop == *current_desktop;
}

bool IsHidden(Display* display, Window window, Atom state_property, Atom hidden_state)
{
    const auto states = ReadCardinalList(display, window, state_property);
    return std::find(states.begin(), states.end(), static_cast<unsigned long>(hidden_state)) != states.end();
}

bool RectanglesIntersect(int left, int top, int width, int height, int monitor_left, int monitor_top, int monitor_width, int monitor_height)
{
    return left + width > monitor_left
        && top + height > monitor_top
        && left < monitor_left + monitor_width
        && top < monitor_top + monitor_height;
}

bool IntersectsScreen(Display* display, Window window, const XWindowAttributes& attributes)
{
    int root_x {};
    int root_y {};
    Window child {};
    if (XTranslateCoordinates(display, window, DefaultRootWindow(display), 0, 0, &root_x, &root_y, &child) == 0) {
        return false;
    }

#if defined(AUTOMATIOTEST_HAS_XINERAMA)
    if (XineramaIsActive(display) != 0) {
        int monitor_count {};
        XineramaScreenInfo* monitors = XineramaQueryScreens(display, &monitor_count);
        if (monitors != nullptr) {
            bool intersects = false;
            for (int index = 0; index < monitor_count && !intersects; ++index) {
                intersects = RectanglesIntersect(
                    root_x,
                    root_y,
                    attributes.width,
                    attributes.height,
                    monitors[index].x_org,
                    monitors[index].y_org,
                    monitors[index].width,
                    monitors[index].height);
            }
            XFree(monitors);
            return intersects;
        }
    }
#endif

    for (int screen_index = 0; screen_index < ScreenCount(display); ++screen_index) {
        const auto* screen = ScreenOfDisplay(display, screen_index);
        if (RectanglesIntersect(root_x, root_y, attributes.width, attributes.height, 0, 0, WidthOfScreen(screen), HeightOfScreen(screen))) {
            return true;
        }
    }
    return false;
}

} // namespace

bool OSRelatedFunctions::IsBrowserVisible(std::unordered_set<std::wstring>* browsers)
{
    if (browsers == nullptr || browsers->empty()) {
        return false;
    }

    std::unordered_set<std::string> normalized_browsers;
    normalized_browsers.reserve(browsers->size());
    for (const auto& browser : *browsers) {
        if (!browser.empty()) {
            normalized_browsers.insert(Lowercase(std::filesystem::path(browser).string()));
        }
    }

    DisplayPointer display(XOpenDisplay(nullptr));
    if (!display) {
        return false;
    }
    const XErrorHandlerGuard error_handler_guard(display.get());

    const auto client_list_property = XInternAtom(display.get(), "_NET_CLIENT_LIST", True);
    const auto process_id_property = XInternAtom(display.get(), "_NET_WM_PID", True);
    const auto state_property = XInternAtom(display.get(), "_NET_WM_STATE", True);
    const auto hidden_state = XInternAtom(display.get(), "_NET_WM_STATE_HIDDEN", True);
    const auto window_desktop_property = XInternAtom(display.get(), "_NET_WM_DESKTOP", True);
    const auto current_desktop_property = XInternAtom(display.get(), "_NET_CURRENT_DESKTOP", True);
    const auto application_id_property = XInternAtom(display.get(), "_GTK_APPLICATION_ID", True);
    if (client_list_property == None) {
        return false;
    }

    const auto windows = ReadCardinalList(display.get(), DefaultRootWindow(display.get()), client_list_property);
    for (const auto value : windows) {
        const auto window = static_cast<Window>(value);
        XWindowAttributes attributes {};
        if (XGetWindowAttributes(display.get(), window, &attributes) == 0
            || attributes.map_state != IsViewable
            || attributes.width <= 0
            || attributes.height <= 0
            || IsHidden(display.get(), window, state_property, hidden_state)
            // || !IsOnCurrentDesktop(display.get(), window, window_desktop_property, current_desktop_property)
            || !IntersectsScreen(display.get(), window, attributes)) {
            continue;
        }

        if (ContainsBrowserIdentifier(display.get(), window, normalized_browsers, process_id_property, application_id_property)) {
            return true;
        }
    }
    return false;
}

#endif
