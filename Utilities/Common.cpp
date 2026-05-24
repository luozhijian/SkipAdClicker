#include "Common.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace automationtest::utilities
{
    void RegisterBindings(utilities::status::LoadFunctions& load_functions)
    {
        using RegisteredMethod = utilities::status::LoadFunctions::RegisteredMethod;
        using RegisteredParameter = utilities::status::LoadFunctions::RegisteredParameter;

        load_functions.RegisterMethod("StoreTrue", RegisteredMethod {
            .declaring_type = "Common",
            .invoke = [](const std::vector<std::any>&) -> std::any {
                return StoreTrue();
            },
        });

        load_functions.RegisterMethod("StoreFalse", RegisteredMethod {
            .declaring_type = "Common",
            .invoke = [](const std::vector<std::any>&) -> std::any {
                return StoreFalse();
            },
        });

        load_functions.RegisterMethod("StoreValue", RegisteredMethod {
            .declaring_type = "Common",
            .invoke = [](const std::vector<std::any>& arguments) -> std::any {
                const auto value = arguments.empty() ? std::nullopt : AnyToDouble(arguments.front());
                return StoreValue(value.value_or(0.0));
            },
            .parameters = {
                RegisteredParameter {.name = "value", .converter = [](const std::string& value) -> std::any {
                    return AnyToDouble(value).value_or(0.0);
                }},
            },
        });
    }


    bool StoreTrue() noexcept
    {
        return true;
    }

    bool StoreFalse() noexcept
    {
        return false;
    }

    double StoreValue(double value) noexcept
    {
        return value;
    }


    std::optional<bool> AnyToBool(const std::any& value)
    {
        if (!value.has_value()) {
            return std::nullopt;
        }
        if (value.type() == typeid(bool)) {
            return std::any_cast<bool>(value);
        }
        if (value.type() == typeid(int)) {
            return std::any_cast<int>(value) != 0;
        }
        if (value.type() == typeid(std::string)) {
            const auto lowered = ToLowerCopy(Trim(std::any_cast<std::string>(value)));
            if (lowered == "true" || lowered == "1") {
                return true;
            }
            if (lowered == "false" || lowered == "0" || lowered.empty()) {
                return false;
            }
        }
        return std::nullopt;
    }

    std::optional<int> AnyToInt(const std::any& value)
    {
        if (!value.has_value()) {
            return std::nullopt;
        }
        if (value.type() == typeid(int)) {
            return std::any_cast<int>(value);
        }
        if (value.type() == typeid(double)) {
            return static_cast<int>(std::any_cast<double>(value));
        }
        if (value.type() == typeid(std::string)) {
            try {
                return std::stoi(Trim(std::any_cast<std::string>(value)));
            }
            catch (...) {
                return std::nullopt;
            }
        }
        return std::nullopt;
    }

    std::optional<double> AnyToDouble(const std::string& value)
    {
        try {
            return std::stod(Trim(value));
        }
        catch (...) {
            return std::nullopt;
        }
    }

    std::optional<double> AnyToDouble(const std::any& value)
    {
        if (!value.has_value()) {
            return std::nullopt;
        }
        if (value.type() == typeid(double)) {
            return std::any_cast<double>(value);
        }
        if (value.type() == typeid(float)) {
            return static_cast<double>(std::any_cast<float>(value));
        }
        if (value.type() == typeid(int)) {
            return static_cast<double>(std::any_cast<int>(value));
        }
        if (value.type() == typeid(std::string)) {
            return AnyToDouble(std::any_cast<std::string>(value));
        }
        return std::nullopt;
    }

    std::string Trim(const std::string& value)
    {
        auto first = std::find_if(value.begin(), value.end(), [](unsigned char character) {
            return !std::isspace(character);
            });
        auto last = std::find_if(value.rbegin(), value.rend(), [](unsigned char character) {
            return !std::isspace(character);
            }).base();
        if (first >= last) {
            return {};
        }
        return std::string(first, last);
    }

    std::string ReplaceEncodedSpaces(const std::string& value)
    {
        std::string result = value;
        std::string::size_type position = 0;
        while ((position = result.find("%20", position)) != std::string::npos) {
            result.replace(position, 3, " ");
            ++position;
        }
        return result;
    }

    std::vector<std::string> SplitActionLine(const std::string& value)
    {
        std::istringstream input(value);
        std::vector<std::string> result{};
        std::string token{};
        while (input >> token) {
            result.push_back(ReplaceEncodedSpaces(token));
        }
        return result;
    }

    std::string ToLowerCopy(const std::string& value)
    {
        std::string result = value;
        std::transform(result.begin(), result.end(), result.begin(), [](unsigned char character) {
            return static_cast<char>(std::tolower(character));
            });
        return result;
    }

    bool StartsWithIgnoreCase(const std::string& value, const std::string& prefix)
    {
        if (prefix.size() > value.size()) {
            return false;
        }
        return ToLowerCopy(value.substr(0, prefix.size())) == ToLowerCopy(prefix);
    }

    bool EndsWithIgnoreCase(const std::string& value, const std::string& suffix)
    {
        if (suffix.size() > value.size()) {
            return false;
        }
        return ToLowerCopy(value.substr(value.size() - suffix.size())) == ToLowerCopy(suffix);
    }
}
