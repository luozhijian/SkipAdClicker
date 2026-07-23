#include "OSRelatedFunctions.hpp"

#if defined(_WIN32)

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <dwmapi.h>
#include <windows.h>

#include <algorithm>
#include <cwctype>
#include <filesystem>
#include <string>
#include <unordered_set>

namespace {

std::wstring Lowercase(std::wstring value)
{
    std::transform(value.begin(), value.end(), value.begin(), [](wchar_t character) {
        return static_cast<wchar_t>(std::towlower(character));
    });
    return value;
}

bool IsConfiguredBrowser(HWND window, const std::unordered_set<std::wstring>& browsers)
{
    DWORD process_id {};
    GetWindowThreadProcessId(window, &process_id);
    if (process_id == 0) {
        return false;
    }

    const HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, process_id);
    if (process == nullptr) {
        return false;
    }

    std::wstring path(32768, L'\0');
    DWORD path_length = static_cast<DWORD>(path.size());
    const bool read_succeeded = QueryFullProcessImageNameW(process, 0, path.data(), &path_length) != FALSE;
    CloseHandle(process);
    if (!read_succeeded) {
        return false;
    }

    path.resize(path_length);
    const auto executable_name = Lowercase(std::filesystem::path(path).filename().wstring());
    return browsers.contains(executable_name);
}

bool IsCloaked(HWND window)
{
    BOOL cloaked = FALSE;
    return SUCCEEDED(DwmGetWindowAttribute(window, DWMWA_CLOAKED, &cloaked, sizeof(cloaked)))
        && cloaked != FALSE;
}

bool GetVisibleWindowRectangle(HWND window, RECT& rectangle)
{
    if (FAILED(DwmGetWindowAttribute(
            window,
            DWMWA_EXTENDED_FRAME_BOUNDS,
            &rectangle,
            sizeof(rectangle)))) {
        if (GetWindowRect(window, &rectangle) == FALSE) {
            return false;
        }
    }

    return rectangle.right > rectangle.left && rectangle.bottom > rectangle.top;
}

bool IsPartiallyTransparent(HWND window)
{
    const LONG_PTR extended_style = GetWindowLongPtrW(window, GWL_EXSTYLE);
    if ((extended_style & WS_EX_LAYERED) == 0) {
        return false;
    }

    COLORREF color_key {};
    BYTE alpha = 255;
    DWORD flags {};
    if (GetLayeredWindowAttributes(window, &color_key, &alpha, &flags) == FALSE) {
        // UpdateLayeredWindow can provide per-pixel alpha without making the
        // uniform alpha value available. Treat that case as transparent.
        return true;
    }

    return (flags & LWA_COLORKEY) != 0
        || ((flags & LWA_ALPHA) != 0 && alpha < 255);
}

bool IsAnyPartUnobscured(HWND window, const RECT& rectangle)
{
    HRGN visible_region = CreateRectRgnIndirect(&rectangle);
    if (visible_region == nullptr) {
        return true;
    }

    for (HWND covering_window = GetWindow(window, GW_HWNDPREV);
         covering_window != nullptr;
         covering_window = GetWindow(covering_window, GW_HWNDPREV)) {
        if (IsWindowVisible(covering_window) == FALSE
            || IsIconic(covering_window) != FALSE
            || IsCloaked(covering_window)
            || IsPartiallyTransparent(covering_window)) {
            continue;
        }

        RECT covering_rectangle {};
        if (!GetVisibleWindowRectangle(covering_window, covering_rectangle)) {
            continue;
        }

        HRGN covering_region = CreateRectRgnIndirect(&covering_rectangle);
        if (covering_region == nullptr) {
            continue;
        }

        const int remaining_region_type = CombineRgn(
            visible_region,
            visible_region,
            covering_region,
            RGN_DIFF);
        DeleteObject(covering_region);

        if (remaining_region_type == NULLREGION) {
            DeleteObject(visible_region);
            return false;
        }
    }

    DeleteObject(visible_region);
    return true;
}

bool IsVisibleOnMonitor(HWND window)
{
    if (IsWindowVisible(window) == FALSE || IsIconic(window) != FALSE) {
        return false;
    }

    if (IsCloaked(window)) {
        return false;
    }

    RECT rectangle {};
    if (!GetVisibleWindowRectangle(window, rectangle)) {
        return false;
    }

    return MonitorFromRect(&rectangle, MONITOR_DEFAULTTONULL) != nullptr
        && IsAnyPartUnobscured(window, rectangle);
}

struct EnumerationContext
{
    const std::unordered_set<std::wstring>* browsers {};
    bool found {};
};

BOOL CALLBACK FindVisibleBrowser(HWND window, LPARAM parameter)
{
    auto& context = *reinterpret_cast<EnumerationContext*>(parameter);
    if (IsConfiguredBrowser(window, *context.browsers) && IsVisibleOnMonitor(window)  ) {
        context.found = true;
        return FALSE;
    }
    return TRUE;
}

} // namespace

bool OSRelatedFunctions::IsBrowserVisible(std::unordered_set<std::wstring>* browsers)
{
    if (browsers == nullptr || browsers->empty()) {
        return false;
    }

    EnumerationContext context {.browsers = browsers };
    EnumWindows(FindVisibleBrowser, reinterpret_cast<LPARAM>(&context));
    return context.found;
}

#endif
