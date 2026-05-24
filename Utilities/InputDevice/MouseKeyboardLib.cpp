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

void MouseHoverAndClickOnPoint2Internal(Point client_point, bool right_click)
{
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
    SleepMilliseconds(500);
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

void MouseKeyboardLib::ClickOnPoint(NativeWindowHandle window_handle, Point client_point)
{
#ifdef _WIN32
    (void)window_handle;
    MouseHoverAndClickOnPoint2Internal(client_point, false);
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
#else
    ThrowWindowsOnly("GetCursorPosition");
#endif
}

bool MouseKeyboardLib::SetCursorPosition(Point point)
{
#ifdef _WIN32
    return ::SetCursorPos(point.x, point.y) != FALSE;
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
