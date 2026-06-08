#ifndef AUTOMATIOTEST_GLOBALSETTING_HPP
#define AUTOMATIOTEST_GLOBALSETTING_HPP

#include <atomic>
#include <string>

namespace automationtest::utilities {

class ManualResetEvent;

class GlobalSetting {
public:
    static inline std::atomic_bool is_stop_test_requested {false};
    static inline double number_in_quote_starts_from {131.0};
    static inline std::string current_running_folder {};
    static inline int screen_refresh_per_millisecond {0};
    static inline int screen_height_per_row {0};
    static inline std::string ctp_file_folder {};
    static inline int screen_rows_to_get {0};
    static inline int width_of_scroll_bar {0};
    static inline std::string ocr_white_list {"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.() "};
    static inline std::string ocr_white_list_all_letter {"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"};
    static inline bool is_batch_mode {false};
    static inline bool save_bitmap_files {true};
    static inline std::string tesseract_engine_language {};
    static inline std::string tesseract_engine_data_folder {};
    static inline float default_text_block_threshold {0.4f};

    static bool StopTest() noexcept;
    static bool ClearStopTest() noexcept;
    static void SetScreenLockBlock();
    static void ClearScreenLockBlock();
    static void WaitForScreenUnlockIfBlocked();
    static bool IsScreenLockBlockSet();
    static std::string ImageFileFolder();
    static void SetImageFileFolder(const std::string& folder);
    static std::string DebugViewImageFileFolder();
    static void SetDebugViewImageFileFolder(const std::string& folder);
    static std::string ToolsToViewBitmap();
    static void SetToolsToViewBitmap(const std::string& tool);

private:
    static inline std::string image_file_folder_ {};
    static inline std::string real_image_file_folder_ {};
    static inline std::string debug_view_image_file_folder_ {};
    static inline std::string real_debug_view_image_file_folder_ {};
    static inline std::string tools_to_view_bitmap_ {};

    static ManualResetEvent& ScreenLockBlockEvent();
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_GLOBALSETTING_HPP
