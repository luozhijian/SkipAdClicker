#include "TestBookLibCommon.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace automationtest::testbooklib {

VariableService*& GlobalVariableService() noexcept
{
    static VariableService* service = nullptr;
    return service;
}

void SetVariableService(VariableService* service) noexcept
{
    GlobalVariableService() = service;
}


} // namespace automationtest::testbooklib
