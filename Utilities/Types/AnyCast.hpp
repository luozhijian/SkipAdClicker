#pragma once

#include <any>
#include <string>
#include <stdexcept>
#include <utility>
#include <vector>

namespace automationtest::utilities::types {
	class AnyCast {
	public:

		template <typename T>
		static T CastOne(const std::vector<std::any> & list, const std::string& function_name) {
			if (list.empty()) {
				throw std::invalid_argument("List is empty when calling: " + function_name);
			}
			return std::any_cast<T>(list[0]);
		}

		template <typename T1, typename T2>
		static std::pair<T1, T2> CastTwo(const std::vector<std::any>& list, const std::string& function_name) {
			if (list.empty() || list.size() < 2) {
				throw std::invalid_argument("List is empty or has insufficient elements when calling: " + function_name);
			}
			return std::make_pair<T1, T2>(std::any_cast<T1>(list[0]), std::any_cast<T2>(list[1]));
		}

	};
};
