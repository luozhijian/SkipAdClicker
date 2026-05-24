#ifndef AUTOMATIOTEST_UTILITIES_REFLECTION_REFLECTION_HPP
#define AUTOMATIOTEST_UTILITIES_REFLECTION_REFLECTION_HPP

#include <array>
#include <concepts>
#include <span>
#include <string_view>
#include <type_traits>

#if defined(__has_include)
#if __has_include(<meta>)
#include <meta>
#define AUTOMATIONTEST_HAS_STD_META 1
#endif
#endif

#if !defined(AUTOMATIONTEST_HAS_STD_META)
#define AUTOMATIONTEST_HAS_STD_META 0
#endif

namespace automationtest::utilities::reflection {

inline constexpr bool HasStandardReflection = AUTOMATIONTEST_HAS_STD_META != 0;

#if AUTOMATIONTEST_HAS_STD_META

using MetaInfo = std::meta::info;

template <typename TObject>
[[nodiscard]] consteval MetaInfo Reflect() noexcept
{
    using Type = std::remove_cvref_t<TObject>;
    return std::meta::dealias(^^Type);
}

[[nodiscard]] consteval std::string_view IdentifierOf(MetaInfo reflection)
{
    return std::meta::identifier_of(reflection);
}

[[nodiscard]] consteval std::string_view DisplayStringOf(MetaInfo reflection)
{
    return std::meta::display_string_of(reflection);
}

template <typename TObject>
[[nodiscard]] consteval auto MembersOf()
{
    return std::meta::members_of(Reflect<TObject>());
}

#else

template <typename TObject>
struct TypeInfo {
    std::string_view name {};
    std::span<const std::string_view> members {};
};

template <typename TObject>
struct ManualReflect {
    static constexpr std::array<std::string_view, 0> member_names {};

    [[nodiscard]] static constexpr TypeInfo<TObject> type_info() noexcept
    {
        return {std::string_view {}, member_names};
    }
};

template <typename TObject>
[[nodiscard]] constexpr TypeInfo<std::remove_cvref_t<TObject>> Reflect() noexcept
{
    return ManualReflect<std::remove_cvref_t<TObject>>::type_info();
}

template <typename TObject>
[[nodiscard]] constexpr TypeInfo<std::remove_cvref_t<TObject>> GetTypeInfo() noexcept
{
    return Reflect<TObject>();
}

template <typename TObject>
concept ReflectedType = requires {
    { ManualReflect<std::remove_cvref_t<TObject>>::type_info() } -> std::same_as<TypeInfo<std::remove_cvref_t<TObject>>>;
};

#endif

} // namespace automationtest::utilities::reflection

#if AUTOMATIONTEST_HAS_STD_META

#define AUTOMATIONTEST_META(ENTITY_OR_TYPE) ^^ENTITY_OR_TYPE
#define AUTOMATIONTEST_META_IDENTIFIER(REFLECTION) ::std::meta::identifier_of(REFLECTION)
#define AUTOMATIONTEST_META_DISPLAY(REFLECTION) ::std::meta::display_string_of(REFLECTION)
#define AUTOMATIONTEST_META_MEMBERS(TYPE_NAME) ::std::meta::members_of(^^TYPE_NAME)
#define AUTOMATIONTEST_META_NONSTATIC_DATA_MEMBERS(TYPE_NAME) ::std::meta::nonstatic_data_members_of(^^TYPE_NAME)
#define AUTOMATIONTEST_REFLECT_TYPE(TYPE_NAME, ...)

#else

#define AUTOMATIONTEST_REFLECT_TYPE(TYPE_NAME, ...) \
    template <> \
    struct automationtest::utilities::reflection::ManualReflect<TYPE_NAME> { \
        static constexpr auto member_names = std::to_array<std::string_view>({__VA_ARGS__}); \
        [[nodiscard]] static constexpr ::automationtest::utilities::reflection::TypeInfo<TYPE_NAME> type_info() noexcept \
        { \
            return {#TYPE_NAME, member_names}; \
        } \
    };

#define AUTOMATIONTEST_REFLECT_EMPTY_TYPE(TYPE_NAME) \
    template <> \
    struct automationtest::utilities::reflection::ManualReflect<TYPE_NAME> { \
        static constexpr std::array<std::string_view, 0> member_names {}; \
        [[nodiscard]] static constexpr ::automationtest::utilities::reflection::TypeInfo<TYPE_NAME> type_info() noexcept \
        { \
            return {#TYPE_NAME, member_names}; \
        } \
    };

#endif

#endif // AUTOMATIOTEST_UTILITIES_REFLECTION_REFLECTION_HPP
