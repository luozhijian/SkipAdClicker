#ifndef AUTOMATIOTEST_PLOYGONWITHDESCRIPTION_HPP
#define AUTOMATIOTEST_PLOYGONWITHDESCRIPTION_HPP

#include <string>

namespace automationtest::utilities {

class PloygonWithDescription {
public:
    virtual ~PloygonWithDescription() = default;

    std::string description {};
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_PLOYGONWITHDESCRIPTION_HPP
