#include "PersistenceSetting.hpp"

namespace automationtest::utilities {

PersistenceSetting& PersistenceSetting::Instance()
{
    static PersistenceSetting instance;
    return instance;
}

} // namespace automationtest::utilities
