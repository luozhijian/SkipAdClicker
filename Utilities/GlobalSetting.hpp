#ifndef AUTOMATIOTEST_GLOBALSETTING_HPP
#define AUTOMATIOTEST_GLOBALSETTING_HPP

#include <string>

namespace automationtest::utilities {

class GlobalSetting {
public:
    static inline bool is_stop_test_requested {false};
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
    static std::string ImageFileFolder();
    static void SetImageFileFolder(const std::string& folder);

private:
    static inline std::string image_file_folder_ {};
    static inline std::string real_image_file_folder_ {};
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_GLOBALSETTING_HPP
