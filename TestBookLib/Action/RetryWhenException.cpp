#include "RetryWhenException.hpp"

namespace automationtest::testbooklib::action {

RetryWhenException::RetryWhenException(int line_number, const std::string& text)
    : BlockAction(line_number, text)
{
}

void RetryWhenException::Parse(const std::vector<std::string>& parts)
{
    retry_count_ = parts.size() > 1 ? std::stoi(parts[1]) : 0;
}

void RetryWhenException::PlayActions(const ActionRunner& runner)
{
    for (int attempt = 0; attempt < retry_count_; ++attempt) {
        try {
            BlockAction::PlayActions(runner);
            break;
        } catch (...) {
            if (attempt + 1 >= retry_count_) {
                throw;
            }
        }
    }
}

} // namespace automationtest::testbooklib::action
