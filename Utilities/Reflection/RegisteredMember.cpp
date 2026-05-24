#include "RegisteredMember.hpp"

#include "JsonValue.hpp"
#include "../FileHelper.hpp"

#include <algorithm>

namespace automationtest::utilities::reflection {

namespace {

bool HasReflectedMember(std::span<const std::string_view> members, std::string_view name)
{
    return std::find(members.begin(), members.end(), name) != members.end();
}

const JsonValue* FindAny(const JsonValue& object, std::initializer_list<std::string_view> names)
{
    for (const auto name : names) {
        if (const auto* value = object.Find(name); value != nullptr) {
            return value;
        }
    }
    return nullptr;
}

void ReadStringMember(const JsonValue& object, std::span<const std::string_view> members, std::string_view member_name, std::string& target, std::initializer_list<std::string_view> aliases = {})
{
    if (!HasReflectedMember(members, member_name)) {
        return;
    }
    if (const auto* value = FindAny(object, aliases.size() == 0 ? std::initializer_list<std::string_view> {member_name} : aliases); value != nullptr) {
        if (auto text = value->AsString(); text.has_value()) {
            target = std::move(text.value());
        }
    }
}

void ReadBoolMember(const JsonValue& object, std::span<const std::string_view> members, std::string_view member_name, bool& target, std::initializer_list<std::string_view> aliases = {})
{
    if (!HasReflectedMember(members, member_name)) {
        return;
    }
    if (const auto* value = FindAny(object, aliases.size() == 0 ? std::initializer_list<std::string_view> {member_name} : aliases); value != nullptr) {
        if (auto boolean = value->AsBool(); boolean.has_value()) {
            target = boolean.value();
        }
    }
}

std::any ToAny(const JsonValue& value)
{
    if (auto text = value.AsString(); text.has_value()) {
        return text.value();
    }
    if (auto boolean = value.AsBool(); boolean.has_value()) {
        return boolean.value();
    }
    if (auto number = value.AsNumber(); number.has_value()) {
        return number.value();
    }
    return {};
}

RegisteredParameter ReadParameter(const JsonValue& value)
{
    RegisteredParameter parameter {};
    const auto info = Reflect<RegisteredParameter>();
    ReadStringMember(value, info.members, "name", parameter.name);
    ReadBoolMember(value, info.members, "has_default_value", parameter.has_default_value, {"has_default_value", "hasDefaultValue", "hasDefault"});
    ReadBoolMember(value, info.members, "is_out", parameter.is_out, {"is_out", "isOut", "out"});
    if (HasReflectedMember(info.members, "default_value")) {
        if (const auto* default_value = FindAny(value, {"default_value", "defaultValue", "default"}); default_value != nullptr && !default_value->IsNull()) {
            parameter.default_value = ToAny(*default_value);
            parameter.has_default_value = true;
        }
    }
    return parameter;
}

RegisteredMethod ReadMethod(const JsonValue& value)
{
    RegisteredMethod method {};
    const auto info = Reflect<RegisteredMethod>();
    ReadStringMember(value, info.members, "name", method.name);
    ReadStringMember(value, info.members, "declaring_type", method.declaring_type, {"declaring_type", "declaringType", "type"});
    ReadBoolMember(value, info.members, "is_static", method.is_static, {"is_static", "isStatic", "static"});

    if (HasReflectedMember(info.members, "parameters")) {
        if (const auto* parameters = FindAny(value, {"parameters", "params"}); parameters != nullptr) {
            if (const auto* array = parameters->AsArray(); array != nullptr) {
                method.parameters.reserve(array->size());
                for (const auto& parameter : *array) {
                    method.parameters.push_back(ReadParameter(parameter));
                }
            }
        }
    }

    return method;
}

} // namespace

std::size_t RegisteredMethod::MinimumParameterCount() const noexcept
{
    std::size_t count = 0;
    for (const auto& parameter : parameters) {
        if (!parameter.is_out && !parameter.has_default_value) {
            ++count;
        }
    }
    return count;
}

std::size_t RegisteredMethod::MaximumParameterCount() const noexcept
{
    std::size_t count = 0;
    for (const auto& parameter : parameters) {
        if (!parameter.is_out) {
            ++count;
        }
    }
    return count;
}

bool RegisteredMethod::Accepts(std::size_t parameter_count) const noexcept
{
    return parameter_count >= MinimumParameterCount() && parameter_count <= MaximumParameterCount();
}

std::optional<RegisteredDll> LoadRegisteredDllFromJson(const std::string& json)
{
    const auto root = ParseJson(json);
    if (!root.has_value() || !root->IsObject()) {
        return std::nullopt;
    }

    RegisteredDll dll {};
    const auto info = Reflect<RegisteredDll>();
    ReadStringMember(root.value(), info.members, "name", dll.name);
    ReadStringMember(root.value(), info.members, "file_path", dll.file_path, {"file_path", "filePath", "path", "dll", "dllPath"});
    ReadStringMember(root.value(), info.members, "description", dll.description);
    ReadStringMember(root.value(), info.members, "version", dll.version);
    ReadStringMember(root.value(), info.members, "company", dll.company);

    if (HasReflectedMember(info.members, "methods")) {
        if (const auto* methods = FindAny(root.value(), {"methods", "registeredMethods"}); methods != nullptr) {
            if (const auto* array = methods->AsArray(); array != nullptr) {
                dll.methods.reserve(array->size());
                for (const auto& method : *array) {
                    dll.methods.push_back(ReadMethod(method));
                }
            }
        }
    }

    return dll;
}

std::optional<RegisteredDll> LoadRegisteredDllFromJsonFile(const std::string& file_path)
{
    auto dll = LoadRegisteredDllFromJson(FileHelper::FileReadAllTextWithLnk(file_path));
    if (dll.has_value() && dll->file_path.empty()) {
        dll->file_path = file_path;
    }
    return dll;
}

} // namespace automationtest::utilities::reflection
