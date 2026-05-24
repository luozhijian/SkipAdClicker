#ifndef AUTOMATIOTEST_REFLECTION_JSONVALUE_HPP
#define AUTOMATIOTEST_REFLECTION_JSONVALUE_HPP

#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace automationtest::utilities::reflection {

class JsonValue {
public:
    using Array = std::vector<JsonValue>;
    using Object = std::map<std::string, JsonValue>;

    JsonValue() = default;
    explicit JsonValue(std::nullptr_t);
    explicit JsonValue(bool value);
    explicit JsonValue(double value);
    explicit JsonValue(std::string value);
    explicit JsonValue(Array value);
    explicit JsonValue(Object value);

    [[nodiscard]] bool IsNull() const noexcept;
    [[nodiscard]] bool IsBool() const noexcept;
    [[nodiscard]] bool IsNumber() const noexcept;
    [[nodiscard]] bool IsString() const noexcept;
    [[nodiscard]] bool IsArray() const noexcept;
    [[nodiscard]] bool IsObject() const noexcept;

    [[nodiscard]] std::optional<bool> AsBool() const;
    [[nodiscard]] std::optional<double> AsNumber() const;
    [[nodiscard]] std::optional<std::string> AsString() const;
    [[nodiscard]] const Array* AsArray() const;
    [[nodiscard]] const Object* AsObject() const;
    [[nodiscard]] const JsonValue* Find(std::string_view key) const;

private:
    std::variant<std::nullptr_t, bool, double, std::string, Array, Object> value_ {nullptr};
};

[[nodiscard]] std::optional<JsonValue> ParseJson(std::string_view json);

} // namespace automationtest::utilities::reflection

#endif // AUTOMATIOTEST_REFLECTION_JSONVALUE_HPP
