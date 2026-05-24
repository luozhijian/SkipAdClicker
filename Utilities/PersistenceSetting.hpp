#ifndef AUTOMATIOTEST_PERSISTENCESETTING_HPP
#define AUTOMATIOTEST_PERSISTENCESETTING_HPP

#include <string>
#include <vector>

namespace automationtest::utilities {

class PersistenceSetting {
public:
    static PersistenceSetting& Instance();

    std::vector<std::string> quote_names {};

private:
    PersistenceSetting() = default;
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_PERSISTENCESETTING_HPP
