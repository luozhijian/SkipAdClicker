#ifndef AUTOMATIOTEST_EDGEWITHDESCRIPTION_HPP
#define AUTOMATIOTEST_EDGEWITHDESCRIPTION_HPP

#include "CommonTypes.hpp"
#include "PloygonWithDescription.hpp"

namespace automationtest::utilities {

struct EdgeWithDescription : public PloygonWithDescription {
    Point point1 {};
    Point point2 {};
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_EDGEWITHDESCRIPTION_HPP
