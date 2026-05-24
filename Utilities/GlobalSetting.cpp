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

} // namespace automationtest::utilities
