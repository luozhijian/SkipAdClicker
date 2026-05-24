#ifndef AUTOMATIOTEST_UTILITIES_COMMON_HPP
#define AUTOMATIOTEST_UTILITIES_COMMON_HPP

#include "CommonTypes.hpp"
#include "./Status/LoadFunctions.hpp"

#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace automationtest::utilities {

	using ClickCallback = std::function<std::optional<Bitmap>(const Point&, int)>;
	using ClickOnlyCallback = std::function<void(const Point&)>;
	using HoverCallback = std::function<std::optional<Bitmap>(const Point&, int)>;
	using TextProvider = std::function<std::string(const Bitmap&)>;

	struct TextMatch {
		Rectangle rectangle {};
		std::string text {};
	};

	std::optional<bool> AnyToBool(const std::any& value);
	std::optional<int> AnyToInt(const std::any& value);
	std::optional<double> AnyToDouble(const std::any& value);
	std::optional<double> AnyToDouble(const std::string& value);
	std::string Trim(const std::string& value);
	std::string ReplaceEncodedSpaces(const std::string& value);
	std::vector<std::string> SplitActionLine(const std::string& value);
	std::string ToLowerCopy(const std::string& value);
	bool StartsWithIgnoreCase(const std::string& value, const std::string& prefix);
	bool EndsWithIgnoreCase(const std::string& value, const std::string& suffix);


	static bool StoreTrue() noexcept;
	static bool StoreFalse() noexcept;
	static double StoreValue(double value) noexcept;


	void RegisterBindings(utilities::status::LoadFunctions& load_functions);
}

namespace automationtest::userinterfacelib {

	using Bitmap = automationtest::utilities::Bitmap;
	using Color = automationtest::utilities::Color;
	using Point = automationtest::utilities::Point;
	using Rectangle = automationtest::utilities::Rectangle;
	using Size = automationtest::utilities::Size;

	using ClickCallback = automationtest::utilities::ClickCallback;
	using ClickOnlyCallback = automationtest::utilities::ClickOnlyCallback;
	using HoverCallback = automationtest::utilities::HoverCallback;
	using TextProvider = automationtest::utilities::TextProvider;
	using TextMatch = automationtest::utilities::TextMatch;

} // namespace automationtest::userinterfacelib

#endif // AUTOMATIOTEST_UTILITIES_COMMON_HPP
