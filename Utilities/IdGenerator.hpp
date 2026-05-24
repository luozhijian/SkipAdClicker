#ifndef AUTOMATIOTEST_IDGENERATOR_HPP
#define AUTOMATIOTEST_IDGENERATOR_HPP

#include <string>

namespace automationtest::utilities {

class IdGenerator {
public:
    static int Id();
    static std::string IdWithDateTime();
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_IDGENERATOR_HPP
