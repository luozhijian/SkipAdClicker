#ifndef AUTOMATIOTEST_TYPES_ENUMONEXCPETIONACTION_HPP
#define AUTOMATIOTEST_TYPES_ENUMONEXCPETIONACTION_HPP

namespace automationtest::utilities::types {

enum class EnumOnExcpetionAction {
    OnErrorRaiseError = 0,
    OnErrorResumeNext = 1,
    OnErrorContinueLoop = 2,
    OnErrorExitLoop = 4,
    OnErrorCountErrorStopAtMax = 8,
    OnErrorCountContinuousErrorStopAtMax = 16
};

} // namespace automationtest::utilities::types

#endif // AUTOMATIOTEST_TYPES_ENUMONEXCPETIONACTION_HPP
