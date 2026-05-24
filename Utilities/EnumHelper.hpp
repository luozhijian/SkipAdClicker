#ifndef AUTOMATIOTEST_ENUMHELPER_HPP
#define AUTOMATIOTEST_ENUMHELPER_HPP

#include "Status/EnumRelativeLocation.hpp"
#include "Types/EnumOnExcpetionAction.hpp"
#include "Types/EnumRelativeLocation.hpp"
#include "Types/EnumSizeDescription.hpp"

#include <algorithm>
#include <array>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace automationtest::utilities {

class EnumHelper {
public:
    static types::EnumRelativeLocation ToEnumRelativeLocation(const std::string& value);

    template <typename T>
    static T TryParseAsEnumWithDefault(const std::string& value, T default_value = {})
    {
        if (value.find_first_not_of(" \t\r\n") == std::string::npos) {
            return default_value;
        }

        const auto entries = EnumTraits<T>::Entries();
        const auto it = std::find_if(entries.begin(), entries.end(), [&value](const auto& entry) {
            return EqualsIgnoreCase(entry.first, value);
        });

        return it == entries.end() ? default_value : it->second;
    }

    template <typename T>
    static std::vector<std::string> GetAllValuesOfEnum()
    {
        std::vector<std::string> values;
        for (const auto& entry : EnumTraits<T>::Entries()) {
            values.push_back(std::string(entry.first));
        }
        return values;
    }

    template <typename T>
    static std::string GetAllValuesAsStringOfEnum()
    {
        std::string result;
        const auto values = GetAllValuesOfEnum<T>();
        for (std::size_t index = 0; index < values.size(); ++index) {
            if (index > 0) {
                result += ',';
            }
            result += values[index];
        }
        return result;
    }

private:
    template <typename T>
    struct EnumTraits;

    static bool EqualsIgnoreCase(std::string_view left, std::string_view right);
};

template <>
struct EnumHelper::EnumTraits<types::EnumRelativeLocation> {
    static constexpr auto Entries()
    {
        using Enum = types::EnumRelativeLocation;
        return std::array {
            std::pair {"None", Enum::None},
            std::pair {"Left", Enum::Left},
            std::pair {"Right", Enum::Right},
            std::pair {"Above", Enum::Above},
            std::pair {"Below", Enum::Below},
            std::pair {"AboveRight", Enum::AboveRight},
            std::pair {"AboveLeft", Enum::AboveLeft},
            std::pair {"BelowRight", Enum::BelowRight},
            std::pair {"BelowLeft", Enum::BelowLeft},
            std::pair {"SameRowLeftClose", Enum::SameRowLeftClose},
            std::pair {"SameRowRightClose", Enum::SameRowRightClose}
        };
    }
};

template <>
struct EnumHelper::EnumTraits<types::EnumOnExcpetionAction> {
    static constexpr auto Entries()
    {
        using Enum = types::EnumOnExcpetionAction;
        return std::array {
            std::pair {"OnErrorRaiseError", Enum::OnErrorRaiseError},
            std::pair {"OnErrorResumeNext", Enum::OnErrorResumeNext},
            std::pair {"OnErrorContinueLoop", Enum::OnErrorContinueLoop},
            std::pair {"OnErrorExitLoop", Enum::OnErrorExitLoop}
        };
    }
};

template <>
struct EnumHelper::EnumTraits<types::EnumSizeDescription> {
    static constexpr auto Entries()
    {
        using Enum = types::EnumSizeDescription;
        return std::array {
            std::pair {"Full", Enum::Full},
            std::pair {"Quarter", Enum::Quarter},
            std::pair {"Half", Enum::Half},
            std::pair {"ThreeQuarter", Enum::ThreeQuarter},
            std::pair {"Dock", Enum::Dock}
        };
    }
};

template <>
struct EnumHelper::EnumTraits<status::EnumRelativeLocation> {
    static constexpr auto Entries()
    {
        using Enum = status::EnumRelativeLocation;
        return std::array {
            std::pair {"LeftOf", Enum::LeftOf},
            std::pair {"RightOf", Enum::RightOf},
            std::pair {"Above", Enum::Above},
            std::pair {"Below", Enum::Below},
            std::pair {"Near", Enum::Near},
            std::pair {"Aligned", Enum::Aligned},
            std::pair {"Overlaps", Enum::Overlaps},
            std::pair {"Inside", Enum::Inside},
            std::pair {"Contains", Enum::Contains},
            std::pair {"Touches", Enum::Touches},
            std::pair {"ZAbove", Enum::ZAbove},
            std::pair {"ZBelow", Enum::ZBelow},
            std::pair {"ImmediateBelow", Enum::ImmediateBelow},
            std::pair {"ImmediateAbove", Enum::ImmediateAbove},
            std::pair {"ImmediateRight", Enum::ImmediateRight},
            std::pair {"ImmediateLeft", Enum::ImmediateLeft},
            std::pair {"SameRow", Enum::SameRow},
            std::pair {"SameColumn", Enum::SameColumn},
            std::pair {"OrderBefore", Enum::OrderBefore},
            std::pair {"OrderAfter", Enum::OrderAfter},
            std::pair {"InGroup", Enum::InGroup},
            std::pair {"LabelFor", Enum::LabelFor},
            std::pair {"DescribedBy", Enum::DescribedBy},
            std::pair {"PlaceholderFor", Enum::PlaceholderFor},
            std::pair {"Decorates", Enum::Decorates},
            std::pair {"HasIcon", Enum::HasIcon},
            std::pair {"SameTextAs", Enum::SameTextAs},
            std::pair {"SimilarText", Enum::SimilarText}
        };
    }
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_ENUMHELPER_HPP
