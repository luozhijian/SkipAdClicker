#include "IdGenerator.hpp"

#include "DateTimeLib.hpp"

#include <atomic>
#include <limits>

namespace automationtest::utilities {

namespace {
std::atomic<int> g_id {0};
}

int IdGenerator::Id()
{
	if (g_id == std::numeric_limits<int>::max()) {
		g_id = 0;
	}
    return ++g_id;
}

std::string IdGenerator::IdWithDateTime()
{
    return DateTimeLib::GetCurrentDateTime_yyyyMMdd_HHmmss() + "_" + std::to_string(Id());
}

} // namespace automationtest::utilities
