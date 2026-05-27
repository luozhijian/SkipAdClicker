#ifndef AUTOMATIOTEST_INPUTDEVICE_MOUSEKEYBOARDLIB_HPP
#define AUTOMATIOTEST_INPUTDEVICE_MOUSEKEYBOARDLIB_HPP

#include "../CommonTypes.hpp"

#include <cstdint>
#include <string>

namespace automationtest::utilities {

class MouseKeyboardLib {
public:
    using NativeWindowHandle = void*;

    enum class VirtualKeyShort : short {
        Shift = 0x10,
        Tab = 0x09,
        Return = 0x0D,
        Delete = 0x2E,
        Next = 0x22
    };

    enum class ScanCodeShort : short {
        Tab = 15,
        Return = 28,
        Shift = 42,
        Next = 81,
        Delete = 83
    };

    static constexpr int WmKeyDown = 0x0100;
    static constexpr int WmKeyUp = 0x0101;
    static constexpr int WmCommand = 0x0111;
    static constexpr int WmLButtonDown = 0x0201;
    static constexpr int WmLButtonUp = 0x0202;
    static constexpr int WmMouseHover = 0x02A1;
    static constexpr int WmLButtonDoubleClick = 0x0203;
    static constexpr int WmRButtonDown = 0x0204;
    static constexpr int WmRButtonUp = 0x0205;
    static constexpr int WmRButtonDoubleClick = 0x0206;
    static constexpr int WmMove = 0x0003;
    static constexpr int WmMouseMove = 0x0200;

    [[nodiscard]] static int CreateLParam(int low_word, int high_word) noexcept;
    static void ClickOnPoint(Point client_point);
    static void ClickOnPointAndRestoreForegroundWindow(Point client_point);
    static void ClickOnPoint(NativeWindowHandle window_handle, Point client_point);
    static void MouseHoverAndClickOnPoint(NativeWindowHandle window_handle, Point client_point);
    static void MouseHoverAndClickOnPoint2(NativeWindowHandle window_handle, Point client_point);
    static void MouseHoverAndRightClickOnPoint(NativeWindowHandle window_handle, Point client_point);
    static void SendInputMouseHoverAndClickOnPoint(NativeWindowHandle window_handle, Point client_point);
    static void ClickAndInputString(NativeWindowHandle window_handle, Point client_point, const std::string& text);
    static void SendInputStringUsingSendInput(const std::string& text);
    static void SendKeyPageDown();
    static void SendKeyEnter();
    static void SendKeyTab();
    static void SendKeyDel();
    static void SendVirtualKey(short virtual_key, int sleep_ms, bool extended_key, bool down, bool up);
    static void SendScanKey(short scan_code, int sleep_ms, bool extended_key, bool down, bool up);
    static void MouseHoverOnPoint(NativeWindowHandle window_handle, Point client_point);
    [[nodiscard]] static Point GetCursorPosition();
    [[nodiscard]] static bool SetCursorPosition(Point point);
    static void DisableMouseAcceleration();

    [[nodiscard]] static NativeWindowHandle FindWindow(const std::string& class_name, const std::string& window_name);
    [[nodiscard]] static NativeWindowHandle FindWindowEx(NativeWindowHandle parent, NativeWindowHandle child_after, const std::string& class_name, const std::string& window_name);
    [[nodiscard]] static bool SetForegroundWindow(NativeWindowHandle window_handle);
    [[nodiscard]] static intptr_t SendMessage(NativeWindowHandle window_handle, int message, intptr_t w_param, intptr_t l_param);
    [[nodiscard]] static intptr_t SendMessage(NativeWindowHandle window_handle, int message, intptr_t w_param, const std::string& l_param);
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_INPUTDEVICE_MOUSEKEYBOARDLIB_HPP
