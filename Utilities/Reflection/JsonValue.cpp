#include "JsonValue.hpp"

#include <cctype>
#include <cstdlib>

namespace automationtest::utilities::reflection {

JsonValue::JsonValue(std::nullptr_t) : value_(nullptr) {}
JsonValue::JsonValue(bool value) : value_(value) {}
JsonValue::JsonValue(double value) : value_(value) {}
JsonValue::JsonValue(std::string value) : value_(std::move(value)) {}
JsonValue::JsonValue(Array value) : value_(std::move(value)) {}
JsonValue::JsonValue(Object value) : value_(std::move(value)) {}

bool JsonValue::IsNull() const noexcept { return std::holds_alternative<std::nullptr_t>(value_); }
bool JsonValue::IsBool() const noexcept { return std::holds_alternative<bool>(value_); }
bool JsonValue::IsNumber() const noexcept { return std::holds_alternative<double>(value_); }
bool JsonValue::IsString() const noexcept { return std::holds_alternative<std::string>(value_); }
bool JsonValue::IsArray() const noexcept { return std::holds_alternative<Array>(value_); }
bool JsonValue::IsObject() const noexcept { return std::holds_alternative<Object>(value_); }

std::optional<bool> JsonValue::AsBool() const
{
    if (const auto* value = std::get_if<bool>(&value_); value != nullptr) {
        return *value;
    }
    return std::nullopt;
}

std::optional<double> JsonValue::AsNumber() const
{
    if (const auto* value = std::get_if<double>(&value_); value != nullptr) {
        return *value;
    }
    return std::nullopt;
}

std::optional<std::string> JsonValue::AsString() const
{
    if (const auto* value = std::get_if<std::string>(&value_); value != nullptr) {
        return *value;
    }
    return std::nullopt;
}

const JsonValue::Array* JsonValue::AsArray() const
{
    return std::get_if<Array>(&value_);
}

const JsonValue::Object* JsonValue::AsObject() const
{
    return std::get_if<Object>(&value_);
}

const JsonValue* JsonValue::Find(std::string_view key) const
{
    const auto* object = AsObject();
    if (object == nullptr) {
        return nullptr;
    }
    const auto iterator = object->find(std::string(key));
    return iterator == object->end() ? nullptr : &iterator->second;
}

namespace {

class JsonParser {
public:
    explicit JsonParser(std::string_view json) : json_(json) {}

    std::optional<JsonValue> Parse()
    {
        SkipWhitespace();
        auto value = ParseValue();
        SkipWhitespace();
        if (!value.has_value() || index_ != json_.size()) {
            return std::nullopt;
        }
        return value;
    }

private:
    void SkipWhitespace()
    {
        while (index_ < json_.size() && std::isspace(static_cast<unsigned char>(json_[index_]))) {
            ++index_;
        }
    }

    bool Consume(char expected)
    {
        SkipWhitespace();
        if (index_ >= json_.size() || json_[index_] != expected) {
            return false;
        }
        ++index_;
        return true;
    }

    std::optional<JsonValue> ParseValue()
    {
        SkipWhitespace();
        if (index_ >= json_.size()) {
            return std::nullopt;
        }
        const char current = json_[index_];
        if (current == '"') {
            auto value = ParseString();
            return value.has_value() ? std::optional<JsonValue>(JsonValue(std::move(value.value()))) : std::nullopt;
        }
        if (current == '{') {
            return ParseObject();
        }
        if (current == '[') {
            return ParseArray();
        }
        if (current == 't' || current == 'f') {
            return ParseBool();
        }
        if (current == 'n') {
            return ParseNull();
        }
        if (current == '-' || std::isdigit(static_cast<unsigned char>(current))) {
            return ParseNumber();
        }
        return std::nullopt;
    }

    std::optional<std::string> ParseString()
    {
        if (!Consume('"')) {
            return std::nullopt;
        }
        std::string result {};
        while (index_ < json_.size()) {
            const char current = json_[index_++];
            if (current == '"') {
                return result;
            }
            if (current != '\\') {
                result.push_back(current);
                continue;
            }
            if (index_ >= json_.size()) {
                return std::nullopt;
            }
            const char escaped = json_[index_++];
            switch (escaped) {
            case '"': result.push_back('"'); break;
            case '\\': result.push_back('\\'); break;
            case '/': result.push_back('/'); break;
            case 'b': result.push_back('\b'); break;
            case 'f': result.push_back('\f'); break;
            case 'n': result.push_back('\n'); break;
            case 'r': result.push_back('\r'); break;
            case 't': result.push_back('\t'); break;
            default: return std::nullopt;
            }
        }
        return std::nullopt;
    }

    std::optional<JsonValue> ParseObject()
    {
        if (!Consume('{')) {
            return std::nullopt;
        }
        JsonValue::Object object {};
        SkipWhitespace();
        if (Consume('}')) {
            return JsonValue(std::move(object));
        }
        while (true) {
            auto key = ParseString();
            if (!key.has_value() || !Consume(':')) {
                return std::nullopt;
            }
            auto value = ParseValue();
            if (!value.has_value()) {
                return std::nullopt;
            }
            object[std::move(key.value())] = std::move(value.value());
            if (Consume('}')) {
                return JsonValue(std::move(object));
            }
            if (!Consume(',')) {
                return std::nullopt;
            }
        }
    }

    std::optional<JsonValue> ParseArray()
    {
        if (!Consume('[')) {
            return std::nullopt;
        }
        JsonValue::Array array {};
        SkipWhitespace();
        if (Consume(']')) {
            return JsonValue(std::move(array));
        }
        while (true) {
            auto value = ParseValue();
            if (!value.has_value()) {
                return std::nullopt;
            }
            array.push_back(std::move(value.value()));
            if (Consume(']')) {
                return JsonValue(std::move(array));
            }
            if (!Consume(',')) {
                return std::nullopt;
            }
        }
    }

    std::optional<JsonValue> ParseBool()
    {
        if (json_.substr(index_, 4) == "true") {
            index_ += 4;
            return JsonValue(true);
        }
        if (json_.substr(index_, 5) == "false") {
            index_ += 5;
            return JsonValue(false);
        }
        return std::nullopt;
    }

    std::optional<JsonValue> ParseNull()
    {
        if (json_.substr(index_, 4) != "null") {
            return std::nullopt;
        }
        index_ += 4;
        return JsonValue(nullptr);
    }

    std::optional<JsonValue> ParseNumber()
    {
        const auto begin = index_;
        if (json_[index_] == '-') {
            ++index_;
        }
        while (index_ < json_.size() && std::isdigit(static_cast<unsigned char>(json_[index_]))) {
            ++index_;
        }
        if (index_ < json_.size() && json_[index_] == '.') {
            ++index_;
            while (index_ < json_.size() && std::isdigit(static_cast<unsigned char>(json_[index_]))) {
                ++index_;
            }
        }
        if (index_ < json_.size() && (json_[index_] == 'e' || json_[index_] == 'E')) {
            ++index_;
            if (index_ < json_.size() && (json_[index_] == '+' || json_[index_] == '-')) {
                ++index_;
            }
            while (index_ < json_.size() && std::isdigit(static_cast<unsigned char>(json_[index_]))) {
                ++index_;
            }
        }
        const auto text = std::string(json_.substr(begin, index_ - begin));
        char* end = nullptr;
        const double value = std::strtod(text.c_str(), &end);
        if (end == text.c_str() || *end != '\0') {
            return std::nullopt;
        }
        return JsonValue(value);
    }

    std::string_view json_;
    std::size_t index_ {0};
};

} // namespace

std::optional<JsonValue> ParseJson(std::string_view json)
{
    return JsonParser(json).Parse();
}

} // namespace automationtest::utilities::reflection
