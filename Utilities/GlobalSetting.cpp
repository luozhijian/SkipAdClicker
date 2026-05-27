#include "GlobalSetting.hpp"

#include "DateTimeLib.hpp"
#include "FilePathLib.hpp"

#include <filesystem>

namespace automationtest::utilities {

bool GlobalSetting::StopTest() noexcept
{
    const bool previous = is_stop_test_requested;
    is_stop_test_requested = true;
    return previous;
}

bool GlobalSetting::ClearStopTest() noexcept
{
    const bool previous = is_stop_test_requested;
    is_stop_test_requested = false;
    return previous;
}

std::string GlobalSetting::ImageFileFolder()
{
    if (!real_image_file_folder_.empty()) {
        return real_image_file_folder_;
    }
    if (image_file_folder_.empty()) {
        return {};
    }
    if (std::filesystem::exists(image_file_folder_)) {
        const auto target = std::filesystem::path(image_file_folder_) / DateTimeLib::GetCurrentDateTime_yyyyMMdd_HHmmss();
        FilePathLib::CreateFolderIfNotExists(target.string());
        real_image_file_folder_ = target.string();
    }
    return real_image_file_folder_;
}

void GlobalSetting::SetImageFileFolder(const std::string& folder)
{
    image_file_folder_ = folder;
    real_image_file_folder_.clear();
}

std::string GlobalSetting::DebugViewImageFileFolder()
{
    if (debug_view_image_file_folder_.empty()) {
        return ImageFileFolder();
    }
    if (!real_debug_view_image_file_folder_.empty()) {
        return real_debug_view_image_file_folder_;
    }

    try {
        const auto target = std::filesystem::path(debug_view_image_file_folder_) / DateTimeLib::GetCurrentDateTime_yyyyMMdd_HHmmss();
        FilePathLib::CreateFolderIfNotExists(target.string());
        real_debug_view_image_file_folder_ = target.string();
    } catch (...) {
        real_debug_view_image_file_folder_.clear();
    }
    return real_debug_view_image_file_folder_;
}

void GlobalSetting::SetDebugViewImageFileFolder(const std::string& folder)
{
    debug_view_image_file_folder_ = folder;
    real_debug_view_image_file_folder_.clear();
}

std::string GlobalSetting::ToolsToViewBitmap()
{
    return tools_to_view_bitmap_;
}

void GlobalSetting::SetToolsToViewBitmap(const std::string& tool)
{
    tools_to_view_bitmap_ = tool;
}

} // namespace automationtest::utilities
