#include "ScrollbarClickWhenException.hpp"

namespace automationtest::testbooklib::action {

ScrollbarClickWhenException::ScrollbarClickWhenException(int line_number, const std::string& text)
    : BlockAction(line_number, text)
{
}

void ScrollbarClickWhenException::Parse(const std::vector<std::string>& parts)
{
    (void)parts;
}

void ScrollbarClickWhenException::PlayActions(const ActionRunner& runner)
{
    BlockAction::PlayActions(runner);
}

} // namespace automationtest::testbooklib::action
