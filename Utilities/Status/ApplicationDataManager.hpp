#ifndef AUTOMATIOTEST_STATUS_APPLICATIONDATAMANAGER_HPP
#define AUTOMATIOTEST_STATUS_APPLICATIONDATAMANAGER_HPP

#include <string>

namespace automationtest::utilities::status {

class ApplicationDataManager {
public:
    static inline std::string application_name {"AutoTestUi"};

    static std::string GetFullfilenameInApplicationData(const std::string& filename);
    static std::string ReadFileInApplicationDataFolder(const std::string& filename);
    static bool WriteFileInApplicationDataFolder(const std::string& filename, const std::string& content);
};

} // namespace automationtest::utilities::status

#endif // AUTOMATIOTEST_STATUS_APPLICATIONDATAMANAGER_HPP
