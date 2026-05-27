#include "MouseKeyboardLib.hpp"

#include "../PointHelper.hpp"

#include <chrono>
#include <stdexcept>
#include <thread>
#include <vector>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#ifdef FindWindow
#undef FindWindow
#endif
#ifdef FindWindowEx
#undef FindWindowEx
#endif
#ifdef SendMessage
#undef SendMessage
#endif
#elif defined(__APPLE__)
#include <ApplicationServices/ApplicationServices.h>
#elif defined(__linux__)
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#endif

namespace automationtest::utilities {

namespace {

void SleepMilliseconds(int milliseconds)
{
    if (milliseconds > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }
}

#ifdef _WIN32

constexpr int InputMouse = 0;
constexpr int InputKeyboard = 1;
constexpr int SmCxScreen = 0;
constexpr int SmCyScreen = 1;
constexpr UINT SpiSetMouseSpeed = 0x0071;
constexpr UINT SpiSetMouse = 0x0004;
constexpr UINT SpiGetMouse = 0x0003;
constexpr UINT SpifSendChange = 0x2;

HWND ToHwnd(MouseKeyboardLib::NativeWindowHandle handle)
{
    return static_cast<HWND>(handle);
}

MouseKeyboardLib::NativeWindowHandle ToNativeWindowHandle(HWND handle)
{
    return static_cast<MouseKeyboardLib::NativeWindowHandle>(handle);
}

int NormalizeAbsoluteCoordinate(int value, int maximum)
{
    if (maximum <= 0) {
        return 0;
    }
    return static_cast<int>(value * 65535.0 / static_cast<double>(maximum));
}

INPUT MakeMouseInput(int x, int y, DWORD flags)
{
    INPUT input {};
    input.type = InputMouse;
    input.mi.dx = x;
    input.mi.dy = y;
    input.mi.dwFlags = flags;
    return input;
}

INPUT MakeKeyboardInput(WORD virtual_key, WORD scan_code, DWORD flags)
{
    INPUT input {};
    input.type = InputKeyboard;
    input.ki.wVk = virtual_key;
    input.ki.wScan = scan_code;
    input.ki.dwFlags = flags;
    return input;
}

void RestoreForegroundWindow(HWND window)
{
    if (window == nullptr || !::IsWindow(window) || window == ::GetForegroundWindow()) {
        return;
    }

    const HWND current_window = ::GetForegroundWindow();
    const DWORD current_thread = current_window != nullptr ? ::GetWindowThreadProcessId(current_window, nullptr) : 0;
    const DWORD target_thread = ::GetWindowThreadProcessId(window, nullptr);
    const DWORD this_thread = ::GetCurrentThreadId();

    const bool attached_current = current_thread != 0 && current_thread != this_thread
        && ::AttachThreadInput(this_thread, current_thread, TRUE) != FALSE;
    const bool attached_target = target_thread != 0 && target_thread != this_thread && target_thread != current_thread
        && ::AttachThreadInput(this_thread, target_thread, TRUE) != FALSE;

    if (::IsIconic(window)) {
        (void)::ShowWindow(window, SW_RESTORE);
    }
    (void)::SetForegroundWindow(window);

    if (attached_target) {
        (void)::AttachThreadInput(this_thread, target_thread, FALSE);
    }
    if (attached_current) {
        (void)::AttachThreadInput(this_thread, current_thread, FALSE);
    }
}

class ForegroundWindowRestorer {
public:
    explicit ForegroundWindowRestorer(bool enabled)
        : window_(enabled ? ::GetForegroundWindow() : nullptr)
    {
    }

    void Restore() const
    {
        RestoreForegroundWindow(window_);
    }

private:
    HWND window_ {nullptr};
};

void SendInputs(const std::vector<INPUT>& inputs)
{
    if (inputs.empty()) {
        return;
    }

    const auto sent = ::SendInput(static_cast<UINT>(inputs.size()), const_cast<INPUT*>(inputs.data()), sizeof(INPUT));
    if (sent != inputs.size()) {
        throw std::runtime_error("SendInput failed.");
    }
}

void SendOneVirtualKey(WORD key)
{
    INPUT input = MakeKeyboardInput(key, 0, 0);
    SendInputs({input});
}

void SendUnicodeChar(wchar_t character)
{
    INPUT down = MakeKeyboardInput(0, static_cast<WORD>(character), KEYEVENTF_UNICODE);
    INPUT up = MakeKeyboardInput(0, static_cast<WORD>(character), KEYEVENTF_UNICODE | KEYEVENTF_KEYUP);
    SendInputs({down, up});
}

void MouseHoverAndClickOnPoint2Internal(Point client_point, bool right_click, bool restore_foreground_after_click = false)
{
    const ForegroundWindowRestorer foreground_window(restore_foreground_after_click);
    const int screen_width = ::GetSystemMetrics(SmCxScreen);
    const int screen_height = ::GetSystemMetrics(SmCyScreen);
    const int x = NormalizeAbsoluteCoordinate(client_point.x, screen_width);
    const int y = NormalizeAbsoluteCoordinate(client_point.y, screen_height);

    const DWORD down_flag = right_click ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_LEFTDOWN;
    const DWORD up_flag = right_click ? MOUSEEVENTF_RIGHTUP : MOUSEEVENTF_LEFTUP;

    SendInputs({
        MakeMouseInput(x, y, MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE),
        MakeMouseInput(x, y, down_flag),
        MakeMouseInput(x, y, up_flag)
    });
    foreground_window.Restore();
    SleepMilliseconds(500);
}

#endif

#ifdef __APPLE__

void PostMouseEvent(CGEventType type, Point point, CGMouseButton button)
{
    const CGPoint cg_point {static_cast<CGFloat>(point.x), static_cast<CGFloat>(point.y)};
    CGEventRef event = CGEventCreateMouseEvent(nullptr, type, cg_point, button);
    if (event == nullptr) {
        throw std::runtime_error("CGEventCreateMouseEvent failed.");
    }
    CGEventPost(kCGHIDEventTap, event);
    CFRelease(event);
}

void MouseClickMac(Point point, bool right_click)
{
    const auto button = right_click ? kCGMouseButtonRight : kCGMouseButtonLeft;
    const auto down = right_click ? kCGEventRightMouseDown : kCGEventLeftMouseDown;
    const auto up = right_click ? kCGEventRightMouseUp : kCGEventLeftMouseUp;
    PostMouseEvent(kCGEventMouseMoved, point, button);
    PostMouseEvent(down, point, button);
    PostMouseEvent(up, point, button);
}

#endif

#ifdef __linux__

class XDisplay {
public:
    XDisplay()
        : display_(XOpenDisplay(nullptr))
    {
        if (display_ == nullptr) {
            throw std::runtime_error("Cannot open X11 display. On Linux, SkipAdClicker currently needs an X11 session or XWayland access.");
        }
    }

    XDisplay(const XDisplay&) = delete;
    XDisplay& operator=(const XDisplay&) = delete;

    ~XDisplay()
    {
        if (display_ != nullptr) {
            XCloseDisplay(display_);
        }
    }

    [[nodiscard]] Display* get() const noexcept
    {
        return display_;
    }

private:
    Display* display_ {};
};

struct XFocusSnapshot {
    Window input_focus {None};
    Window activation_window {None};
    int revert_to {RevertToParent};
    bool valid {false};
};

bool HasWindowProperty(Display* display, Window window, Atom property)
{
    if (property == None) {
        return false;
    }

    Atom actual_type {};
    int actual_format {};
    unsigned long item_count {};
    unsigned long bytes_after {};
    unsigned char* property_data {};
    const int status = XGetWindowProperty(
        display,
        window,
        property,
        0,
        0,
        False,
        AnyPropertyType,
        &actual_type,
        &actual_format,
        &item_count,
        &bytes_after,
        &property_data);
    if (property_data != nullptr) {
        XFree(property_data);
    }
    return status == Success && actual_type != None;
}

Window GetActivationWindow(Display* display, Window window)
{
    if (display == nullptr || window == None || window == PointerRoot) {
        return None;
    }

    const Window root = DefaultRootWindow(display);
    const Atom wm_state = XInternAtom(display, "WM_STATE", True);
    Window current = window;
    while (current != None && current != root) {
        if (HasWindowProperty(display, current, wm_state)) {
            return current;
        }

        Window returned_root {};
        Window parent {};
        Window* children {};
        unsigned int child_count {};
        if (XQueryTree(display, current, &returned_root, &parent, &children, &child_count) == 0) {
            return current;
        }
        if (children != nullptr) {
            XFree(children);
        }
        if (parent == None || parent == root) {
            return current;
        }
        current = parent;
    }

    return None;
}

XFocusSnapshot CaptureXFocus(Display* display)
{
    XFocusSnapshot snapshot {};
    XGetInputFocus(display, &snapshot.input_focus, &snapshot.revert_to);
    if (snapshot.input_focus == None || snapshot.input_focus == PointerRoot) {
        return snapshot;
    }

    snapshot.activation_window = GetActivationWindow(display, snapshot.input_focus);
    snapshot.valid = true;
    return snapshot;
}

void RequestXActiveWindow(Display* display, Window window)
{
    if (display == nullptr || window == None) {
        return;
    }

    const Atom active_window = XInternAtom(display, "_NET_ACTIVE_WINDOW", False);
    if (active_window == None) {
        return;
    }

    XEvent event {};
    event.xclient.type = ClientMessage;
    event.xclient.window = window;
    event.xclient.message_type = active_window;
    event.xclient.format = 32;
    event.xclient.data.l[0] = 1;
    event.xclient.data.l[1] = CurrentTime;
    event.xclient.data.l[2] = None;

    XSendEvent(
        display,
        DefaultRootWindow(display),
        False,
        SubstructureRedirectMask | SubstructureNotifyMask,
        &event);
}

void RestoreXFocus(Display* display, const XFocusSnapshot& snapshot)
{
    if (display == nullptr || !snapshot.valid) {
        return;
    }

    if (snapshot.activation_window != None) {
        RequestXActiveWindow(display, snapshot.activation_window);
    }
    XSetInputFocus(display, snapshot.input_focus, snapshot.revert_to, CurrentTime);
    XFlush(display);
}

void MouseClickX11(Point point, bool right_click, bool restore_focus_after_click = false)
{
    XDisplay display;
    const auto saved_focus = restore_focus_after_click ? CaptureXFocus(display.get()) : XFocusSnapshot {};
    const int button = right_click ? 3 : 1;
    const int screen = DefaultScreen(display.get());
    XTestFakeMotionEvent(display.get(), screen, point.x, point.y, CurrentTime);
    XTestFakeButtonEvent(display.get(), button, True, CurrentTime);
    XTestFakeButtonEvent(display.get(), button, False, CurrentTime);
    if (restore_focus_after_click) {
        XSync(display.get(), False);
        SleepMilliseconds(100);
        RestoreXFocus(display.get(), saved_focus);
        return;
    }
    XFlush(display.get());
}

#endif

void ThrowWindowsOnly(const char* function_name)
{
    throw std::runtime_error(std::string(function_name) + " is only available on Windows.");
}

} // namespace

int MouseKeyboardLib::CreateLParam(int low_word, int high_word) noexcept
{
    return (high_word << 16) | (low_word & 0xffff);
}

void MouseKeyboardLib::ClickOnPoint(Point client_point)
{
    ClickOnPoint(nullptr, client_point);
}

void MouseKeyboardLib::ClickOnPointAndRestoreForegroundWindow(Point client_point)
{
#ifdef _WIN32
    MouseHoverAndClickOnPoint2Internal(client_point, false, true);
#elif defined(__linux__)
    MouseClickX11(client_point, false, true);
#else
    ClickOnPoint(client_point);
#endif
}

void MouseKeyboardLib::ClickOnPoint(NativeWindowHandle window_handle, Point client_point)
{
#ifdef _WIN32
    (void)window_handle;
    MouseHoverAndClickOnPoint2Internal(client_point, false);
#elif defined(__APPLE__)
    (void)window_handle;
    MouseClickMac(client_point, false);
#elif defined(__linux__)
    (void)window_handle;
    MouseClickX11(client_point, false);
#else
    (void)window_handle;
    (void)client_point;
    ThrowWindowsOnly("ClickOnPoint");
#endif
}

void MouseKeyboardLib::MouseHoverAndClickOnPoint(NativeWindowHandle window_handle, Point client_point)
{
#ifdef _WIN32
    const auto location = CreateLParam(client_point.x, client_point.y);
    const HWND hwnd = ToHwnd(window_handle);
    (void)::SendMessageA(hwnd, WmMouseMove, 1, location);
    (void)::SendMessageA(hwnd, WmMouseMove, 1, CreateLParam(client_point.x + 1, client_point.y + 1));
    SleepMilliseconds(200);
    (void)::SendMessageA(hwnd, WmLButtonDown, 1, location);
    SleepMilliseconds(200);
    (void)::SendMessageA(hwnd, WmLButtonUp, 1, location);
    SleepMilliseconds(1000);
#else
    (void)window_handle;
    (void)client_point;
    ThrowWindowsOnly("MouseHoverAndClickOnPoint");
#endif
}

void MouseKeyboardLib::MouseHoverAndClickOnPoint2(NativeWindowHandle window_handle, Point client_point)
{
#ifdef _WIN32
    (void)window_handle;
    MouseHoverAndClickOnPoint2Internal(client_point, false);
#elif defined(__APPLE__) || defined(__linux__)
    ClickOnPoint(window_handle, client_point);
#else
    (void)window_handle;
    (void)client_point;
    ThrowWindowsOnly("MouseHoverAndClickOnPoint2");
#endif
}

void MouseKeyboardLib::MouseHoverAndRightClickOnPoint(NativeWindowHandle window_handle, Point client_point)
{
#ifdef _WIN32
    (void)window_handle;
    MouseHoverAndClickOnPoint2Internal(client_point, true);
#elif defined(__APPLE__)
    (void)window_handle;
    MouseClickMac(client_point, true);
#elif defined(__linux__)
    (void)window_handle;
    MouseClickX11(client_point, true);
#else
    (void)window_handle;
    (void)client_point;
    ThrowWindowsOnly("MouseHoverAndRightClickOnPoint");
#endif
}

void MouseKeyboardLib::SendInputMouseHoverAndClickOnPoint(NativeWindowHandle window_handle, Point client_point)
{
#ifdef _WIN32
    (void)window_handle;
    SendInputs({
        MakeMouseInput(client_point.x, client_point.y, MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE),
        MakeMouseInput(client_point.x, client_point.y, MOUSEEVENTF_LEFTDOWN),
        MakeMouseInput(client_point.x, client_point.y, MOUSEEVENTF_LEFTUP)
    });
    SleepMilliseconds(2000);
#else
    (void)window_handle;
    (void)client_point;
    ThrowWindowsOnly("SendInputMouseHoverAndClickOnPoint");
#endif
}

void MouseKeyboardLib::ClickAndInputString(NativeWindowHandle window_handle, Point client_point, const std::string& text)
{
    ClickOnPoint(window_handle, client_point);
    SleepMilliseconds(20);
    SendInputStringUsingSendInput(text);
}

void MouseKeyboardLib::SendInputStringUsingSendInput(const std::string& text)
{
#ifdef _WIN32
    if (text.empty()) {
        return;
    }

    const HKL keyboard_layout = ::GetKeyboardLayout(0);
    for (const unsigned char character : text) {
        if (character == '\n') {
            SendKeyEnter();
            continue;
        }
        if (character == '\t') {
            SendKeyTab();
            continue;
        }

        const SHORT scan = ::VkKeyScanExA(static_cast<CHAR>(character), keyboard_layout);
        if (scan == -1) {
            SendUnicodeChar(static_cast<wchar_t>(character));
            continue;
        }

        const BYTE modifiers = static_cast<BYTE>((scan >> 8) & 0xff);
        const BYTE virtual_key = static_cast<BYTE>(scan & 0xff);
        const bool shift = (modifiers & 0x01) != 0;

        if (shift) {
            SendVirtualKey(static_cast<short>(VirtualKeyShort::Shift), 10, false, true, false);
        }
        SendVirtualKey(virtual_key, 20, false, true, true);
        if (shift) {
            SendVirtualKey(static_cast<short>(VirtualKeyShort::Shift), 10, false, false, true);
        }
    }
#else
    (void)text;
    ThrowWindowsOnly("SendInputStringUsingSendInput");
#endif
}

void MouseKeyboardLib::SendKeyPageDown()
{
    SendVirtualKey(static_cast<short>(VirtualKeyShort::Next), 0, false, true, true);
}

void MouseKeyboardLib::SendKeyEnter()
{
    SendVirtualKey(static_cast<short>(VirtualKeyShort::Return), 0, false, true, true);
}

void MouseKeyboardLib::SendKeyTab()
{
    SendVirtualKey(static_cast<short>(VirtualKeyShort::Tab), 0, false, true, true);
}

void MouseKeyboardLib::SendKeyDel()
{
    SendVirtualKey(static_cast<short>(VirtualKeyShort::Delete), 0, false, true, true);
}

void MouseKeyboardLib::SendVirtualKey(short virtual_key, int sleep_ms, bool extended_key, bool down, bool up)
{
#ifdef _WIN32
    const auto vk = static_cast<WORD>(virtual_key);
    const auto scan = static_cast<WORD>(::MapVirtualKeyW(vk, MAPVK_VK_TO_VSC));
    if (down) {
        DWORD flags = KEYEVENTF_SCANCODE;
        if (extended_key) {
            flags |= KEYEVENTF_EXTENDEDKEY;
        }
        SendInputs({MakeKeyboardInput(vk, scan, flags)});
        SleepMilliseconds(sleep_ms);
    }
    if (up) {
        DWORD flags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
        if (extended_key) {
            flags |= KEYEVENTF_EXTENDEDKEY;
        }
        SendInputs({MakeKeyboardInput(vk, scan, flags)});
        SleepMilliseconds(sleep_ms);
    }
#else
    (void)virtual_key;
    (void)sleep_ms;
    (void)extended_key;
    (void)down;
    (void)up;
    ThrowWindowsOnly("SendVirtualKey");
#endif
}

void MouseKeyboardLib::SendScanKey(short scan_code, int sleep_ms, bool extended_key, bool down, bool up)
{
#ifdef _WIN32
    const auto scan = static_cast<WORD>(scan_code);
    const auto vk = static_cast<WORD>(::MapVirtualKeyW(scan, MAPVK_VSC_TO_VK));
    if (down) {
        DWORD flags = KEYEVENTF_SCANCODE;
        if (extended_key) {
            flags |= KEYEVENTF_EXTENDEDKEY;
        }
        SendInputs({MakeKeyboardInput(vk, scan, flags)});
        SleepMilliseconds(sleep_ms);
    }
    if (up) {
        DWORD flags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
        if (extended_key) {
            flags |= KEYEVENTF_EXTENDEDKEY;
        }
        SendInputs({MakeKeyboardInput(vk, scan, flags)});
        SleepMilliseconds(sleep_ms);
    }
#else
    (void)scan_code;
    (void)sleep_ms;
    (void)extended_key;
    (void)down;
    (void)up;
    ThrowWindowsOnly("SendScanKey");
#endif
}

void MouseKeyboardLib::MouseHoverOnPoint(NativeWindowHandle window_handle, Point client_point)
{
#ifdef _WIN32
    POINT screen_point {client_point.x, client_point.y};
    const HWND hwnd = ToHwnd(window_handle);
    if (hwnd != nullptr) {
        ::ClientToScreen(hwnd, &screen_point);
    }

    auto old_position = GetCursorPosition();
    const Point target {screen_point.x, screen_point.y};
    if (!PointHelper::TwoPointsAreFaraway(old_position, target)) {
        old_position = Point {0, target.y};
    }

    const int screen_width = ::GetSystemMetrics(SmCxScreen);
    const int screen_height = ::GetSystemMetrics(SmCyScreen);
    std::vector<INPUT> inputs;
    for (const auto& point : PointHelper::GeneratePointsFromPoint1ToPoint2(old_position, target)) {
        inputs.push_back(MakeMouseInput(
            NormalizeAbsoluteCoordinate(point.x, screen_width),
            NormalizeAbsoluteCoordinate(point.y, screen_height),
            MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE));
    }
    SendInputs(inputs);
    SleepMilliseconds(10);
#else
    (void)window_handle;
    (void)client_point;
    ThrowWindowsOnly("MouseHoverOnPoint");
#endif
}

Point MouseKeyboardLib::GetCursorPosition()
{
#ifdef _WIN32
    POINT point {};
    if (!::GetCursorPos(&point)) {
        throw std::runtime_error("GetCursorPos failed.");
    }
    return Point {point.x, point.y};
#elif defined(__APPLE__)
    CGEventRef event = CGEventCreate(nullptr);
    if (event == nullptr) {
        throw std::runtime_error("CGEventCreate failed.");
    }
    const CGPoint location = CGEventGetLocation(event);
    CFRelease(event);
    return Point {static_cast<int>(location.x), static_cast<int>(location.y)};
#elif defined(__linux__)
    XDisplay display;
    Window root = DefaultRootWindow(display.get());
    Window returned_root {};
    Window returned_child {};
    int root_x {};
    int root_y {};
    int window_x {};
    int window_y {};
    unsigned int mask {};
    if (!XQueryPointer(display.get(), root, &returned_root, &returned_child, &root_x, &root_y, &window_x, &window_y, &mask)) {
        throw std::runtime_error("XQueryPointer failed.");
    }
    return Point {root_x, root_y};
#else
    ThrowWindowsOnly("GetCursorPosition");
#endif
}

bool MouseKeyboardLib::SetCursorPosition(Point point)
{
#ifdef _WIN32
    return ::SetCursorPos(point.x, point.y) != FALSE;
#elif defined(__APPLE__)
    return CGWarpMouseCursorPosition(CGPoint {static_cast<CGFloat>(point.x), static_cast<CGFloat>(point.y)}) == kCGErrorSuccess;
#elif defined(__linux__)
    XDisplay display;
    XWarpPointer(display.get(), None, DefaultRootWindow(display.get()), 0, 0, 0, 0, point.x, point.y);
    XFlush(display.get());
    return true;
#else
    (void)point;
    return false;
#endif
}

void MouseKeyboardLib::DisableMouseAcceleration()
{
#ifdef _WIN32
    int mouse_parameters[3] {0, 0, 0};
    (void)::SystemParametersInfoW(SpiGetMouse, 0, mouse_parameters, 0);

    int disabled_parameters[3] {0, 0, 0};
    if (!::SystemParametersInfoW(SpiSetMouse, 0, disabled_parameters, SpifSendChange)) {
        throw std::runtime_error("SPI_SETMOUSE failed.");
    }

    if (!::SystemParametersInfoW(SpiSetMouseSpeed, 0, reinterpret_cast<PVOID>(10), 0)) {
        throw std::runtime_error("SPI_SETMOUSESPEED failed.");
    }
#else
    ThrowWindowsOnly("DisableMouseAcceleration");
#endif
}

MouseKeyboardLib::NativeWindowHandle MouseKeyboardLib::FindWindow(const std::string& class_name, const std::string& window_name)
{
#ifdef _WIN32
    return ToNativeWindowHandle(::FindWindowA(class_name.empty() ? nullptr : class_name.c_str(), window_name.empty() ? nullptr : window_name.c_str()));
#else
    (void)class_name;
    (void)window_name;
    return nullptr;
#endif
}

MouseKeyboardLib::NativeWindowHandle MouseKeyboardLib::FindWindowEx(NativeWindowHandle parent, NativeWindowHandle child_after, const std::string& class_name, const std::string& window_name)
{
#ifdef _WIN32
    return ToNativeWindowHandle(::FindWindowExA(
        ToHwnd(parent),
        ToHwnd(child_after),
        class_name.empty() ? nullptr : class_name.c_str(),
        window_name.empty() ? nullptr : window_name.c_str()));
#else
    (void)parent;
    (void)child_after;
    (void)class_name;
    (void)window_name;
    return nullptr;
#endif
}

bool MouseKeyboardLib::SetForegroundWindow(NativeWindowHandle window_handle)
{
#ifdef _WIN32
    return ::SetForegroundWindow(ToHwnd(window_handle)) != FALSE;
#else
    (void)window_handle;
    return false;
#endif
}

intptr_t MouseKeyboardLib::SendMessage(NativeWindowHandle window_handle, int message, intptr_t w_param, intptr_t l_param)
{
#ifdef _WIN32
    return static_cast<intptr_t>(::SendMessageA(ToHwnd(window_handle), static_cast<UINT>(message), static_cast<WPARAM>(w_param), static_cast<LPARAM>(l_param)));
#else
    (void)window_handle;
    (void)message;
    (void)w_param;
    (void)l_param;
    return 0;
#endif
}

intptr_t MouseKeyboardLib::SendMessage(NativeWindowHandle window_handle, int message, intptr_t w_param, const std::string& l_param)
{
#ifdef _WIN32
    return static_cast<intptr_t>(::SendMessageA(ToHwnd(window_handle), static_cast<UINT>(message), static_cast<WPARAM>(w_param), reinterpret_cast<LPARAM>(l_param.c_str())));
#else
    (void)window_handle;
    (void)message;
    (void)w_param;
    (void)l_param;
    return 0;
#endif
}

} // namespace automationtest::utilities
