#include "OSRelatedFunctions.hpp"

#if defined(__APPLE__)

#import <AppKit/AppKit.h>
#include <ApplicationServices/ApplicationServices.h>

#include <algorithm>
#include <filesystem>
#include <string>
#include <unordered_set>
#include <vector>

namespace {

bool IntersectsActiveDisplay(CGRect rectangle)
{
    uint32_t display_count {};
    if (CGGetActiveDisplayList(0, nullptr, &display_count) != kCGErrorSuccess || display_count == 0) {
        return false;
    }

    std::vector<CGDirectDisplayID> displays(display_count);
    if (CGGetActiveDisplayList(display_count, displays.data(), &display_count) != kCGErrorSuccess) {
        return false;
    }

    return std::any_of(displays.begin(), displays.begin() + display_count, [&](CGDirectDisplayID display) {
        return CGRectIntersectsRect(rectangle, CGDisplayBounds(display));
    });
}

bool IsConfiguredBrowser(pid_t process_id, const std::unordered_set<std::string>& browsers)
{
    NSRunningApplication* application = [NSRunningApplication runningApplicationWithProcessIdentifier:process_id];
    NSString* bundle_identifier = application.bundleIdentifier;
    if (bundle_identifier == nil) {
        return false;
    }

    const char* utf8_identifier = bundle_identifier.UTF8String;
    if (utf8_identifier == nullptr) {
        return false;
    }
    return browsers.contains(utf8_identifier);
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
        normalized_browsers.insert(std::filesystem::path(browser).string());
    }

    @autoreleasepool {
        CFArrayRef window_list = CGWindowListCopyWindowInfo(
            kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements,
            kCGNullWindowID);
        if (window_list == nullptr) {
            return false;
        }

        bool found = false;
        const CFIndex window_count = CFArrayGetCount(window_list);
        for (CFIndex index = 0; index < window_count && !found; ++index) {
            auto* window = static_cast<CFDictionaryRef>(CFArrayGetValueAtIndex(window_list, index));

            int layer {};
            double alpha {};
            int process_id {};
            CGRect bounds {};
            const auto* layer_value = static_cast<CFNumberRef>(CFDictionaryGetValue(window, kCGWindowLayer));
            const auto* alpha_value = static_cast<CFNumberRef>(CFDictionaryGetValue(window, kCGWindowAlpha));
            const auto* process_id_value = static_cast<CFNumberRef>(CFDictionaryGetValue(window, kCGWindowOwnerPID));
            const auto* bounds_value = static_cast<CFDictionaryRef>(CFDictionaryGetValue(window, kCGWindowBounds));
            if (layer_value == nullptr
                || alpha_value == nullptr
                || process_id_value == nullptr
                || bounds_value == nullptr
                || !CFNumberGetValue(layer_value, kCFNumberIntType, &layer)
                || !CFNumberGetValue(alpha_value, kCFNumberDoubleType, &alpha)
                || !CFNumberGetValue(process_id_value, kCFNumberIntType, &process_id)
                || !CGRectMakeWithDictionaryRepresentation(bounds_value, &bounds)
                || layer != 0
                || alpha <= 0.0
                || CGRectIsEmpty(bounds)
                || !IntersectsActiveDisplay(bounds)) {
                continue;
            }

            found = IsConfiguredBrowser(static_cast<pid_t>(process_id), normalized_browsers);
        }

        CFRelease(window_list);
        return found;
    }
}

#endif
