#ifndef AUTOMATIOTEST_TESTBOOKLIB_COMMON_HPP
#define AUTOMATIOTEST_TESTBOOKLIB_COMMON_HPP

#include "../Utilities/Interface/IVariableService.hpp"
#include "../Utilities/Common.hpp"

#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace automationtest::testbooklib {

using VariableService = automationtest::utilities::interface::IVariableService;
using automationtest::utilities::AnyToBool;
using automationtest::utilities::AnyToDouble;
using automationtest::utilities::AnyToInt;
using automationtest::utilities::EndsWithIgnoreCase;
using automationtest::utilities::ReplaceEncodedSpaces;
using automationtest::utilities::SplitActionLine;
using automationtest::utilities::StartsWithIgnoreCase;
using automationtest::utilities::ToLowerCopy;
using automationtest::utilities::Trim;

class TestAction;
using ActionRunner = std::function<void(TestAction&)>;
using VariableServiceProvider = std::function<VariableService*()>;

VariableService*& GlobalVariableService() noexcept;
void SetVariableService(VariableService* service) noexcept;


} // namespace automationtest::testbooklib

#endif // AUTOMATIOTEST_TESTBOOKLIB_COMMON_HPP
