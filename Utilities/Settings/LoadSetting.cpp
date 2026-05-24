#include "LoadSetting.hpp"

#include "../FileHelper.hpp"
#include "../Reflection/JsonValue.hpp"
#include "../Services/VariableService.hpp"
#include "../Status/LoadFunctions.hpp"
#include "SettingCanny.hpp"
#include "SettingFindIcon.hpp"
#include "SettingFormatSet.hpp"
#include "SettingLineDetection.hpp"
#include "SettingLoadMatGray.hpp"
#include "SettingRectangleDetection.hpp"
#include "SettingSqlblock.hpp"
#include "SettingTextBlock.hpp"
#include "SettingThreshold.hpp"
#include "SettingTriangleDetection.hpp"

#include <algorithm>
#include <any>
#include <cctype>
#include <iostream>
#include <optional>
#include <string_view>
#include <utility>

namespace automationtest::utilities::settings {

namespace {

using automationtest::utilities::reflection::JsonValue;

std::string Normalize(std::string_view value)
{
    std::string normalized;
    normalized.reserve(value.size());
    for (const unsigned char ch : value) {
        if (ch == '_' || ch == '-') {
            continue;
        }
        normalized.push_back(static_cast<char>(std::tolower(ch)));
    }
    return normalized;
}

const JsonValue* FindProperty(const JsonValue& value, std::string_view name)
{
    if (const auto* exact = value.Find(name); exact != nullptr) {
        return exact;
    }

    const auto* object = value.AsObject();
    if (object == nullptr) {
        return nullptr;
    }

    const auto target = Normalize(name);
    for (const auto& [key, child] : *object) {
        if (Normalize(key) == target) {
            return &child;
        }
    }
    return nullptr;
}

std::optional<std::string> GetString(const JsonValue& value, std::string_view name)
{
    const auto* property = FindProperty(value, name);
    return property == nullptr ? std::nullopt : property->AsString();
}

std::optional<int> GetInt(const JsonValue& value, std::string_view name)
{
    const auto* property = FindProperty(value, name);
    const auto number = property == nullptr ? std::nullopt : property->AsNumber();
    return number.has_value() ? std::optional<int>(static_cast<int>(*number)) : std::nullopt;
}

std::optional<double> GetDouble(const JsonValue& value, std::string_view name)
{
    const auto* property = FindProperty(value, name);
    return property == nullptr ? std::nullopt : property->AsNumber();
}

std::optional<bool> GetBool(const JsonValue& value, std::string_view name)
{
    const auto* property = FindProperty(value, name);
    return property == nullptr ? std::nullopt : property->AsBool();
}

SettingCanny ReadSettingCanny(const JsonValue& root)
{
    SettingCanny setting;
    setting.threshold1 = GetInt(root, "Threshold1").value_or(setting.threshold1);
    setting.threshold2 = GetInt(root, "Threshold2").value_or(setting.threshold2);
    setting.aperture_size = GetInt(root, "ApertureSize").value_or(setting.aperture_size);
    setting.l2_gradient = GetBool(root, "L2gradient").value_or(setting.l2_gradient);
    return setting;
}

SettingThreshold ReadSettingThreshold(const JsonValue& root)
{
    SettingThreshold setting;
    setting.apply_threshold = GetString(root, "ApplyThreshold").value_or(setting.apply_threshold);
    setting.threshold_value = GetInt(root, "ThresholdValue").value_or(setting.threshold_value);
    setting.threshold_max_value = GetInt(root, "ThresholdMaxValue").value_or(setting.threshold_max_value);
    return setting;
}

SettingLoadMatGray ReadSettingLoadMatGray(const JsonValue& root)
{
    SettingLoadMatGray setting;
    setting.apply_threshold = GetString(root, "ApplyThreshold").value_or(setting.apply_threshold);
    setting.threshold_value = GetInt(root, "ThresholdValue").value_or(setting.threshold_value);
    setting.threshold_max_value = GetInt(root, "ThresholdMaxValue").value_or(setting.threshold_max_value);
    return setting;
}

SettingFindIcon ReadSettingFindIcon(const JsonValue& root)
{
    SettingFindIcon setting;
    setting.apply_threshold = GetString(root, "ApplyThreshold").value_or(setting.apply_threshold);
    setting.threshold_value = GetInt(root, "ThresholdValue").value_or(setting.threshold_value);
    setting.threshold_max_value = GetInt(root, "ThresholdMaxValue").value_or(setting.threshold_max_value);
    setting.scales = GetString(root, "Scales").value_or(setting.scales);
    setting.threshold = GetDouble(root, "Threshold").value_or(setting.threshold);
    if (const auto* canny = FindProperty(root, "SettingCanny"); canny != nullptr) {
        setting.setting_canny = ReadSettingCanny(*canny);
    }
    return setting;
}

SettingLineDetection ReadSettingLineDetection(const JsonValue& root)
{
    SettingLineDetection setting;
    if (const auto* canny = FindProperty(root, "SettingCanny"); canny != nullptr) {
        setting.setting_canny = ReadSettingCanny(*canny);
    }
    if (const auto* threshold = FindProperty(root, "SettingThreshold"); threshold != nullptr) {
        setting.setting_threshold = ReadSettingThreshold(*threshold);
    }
    setting.angle_resolution_in_radians = GetDouble(root, "AngleResolutionInRadians").value_or(setting.angle_resolution_in_radians);
    setting.hough_lines_threshold = GetInt(root, "HoughLinesThreshold").value_or(setting.hough_lines_threshold);
    setting.hough_lines_min_line_length = GetInt(root, "HoughLinesMinLineLength").value_or(setting.hough_lines_min_line_length);
    setting.hough_lines_max_line_gap = GetInt(root, "HoughLinesMaxLineGap").value_or(setting.hough_lines_max_line_gap);
    return setting;
}

SettingRectangleDetection ReadSettingRectangleDetection(const JsonValue& root)
{
    SettingRectangleDetection setting;
    setting.min_area = GetInt(root, "MinArea").value_or(setting.min_area);
    setting.max_area = GetInt(root, "MaxArea").value_or(setting.max_area);
    setting.min_height = GetInt(root, "MinHeight").value_or(setting.min_height);
    setting.max_height = GetInt(root, "MaxHeight").value_or(setting.max_height);
    setting.min_width = GetInt(root, "MinWidth").value_or(setting.min_width);
    setting.max_width = GetInt(root, "MaxWidth").value_or(setting.max_width);
    setting.canny_threshold = GetInt(root, "CannyThreshold").value_or(setting.canny_threshold);
    setting.canny_threshold_linking = GetInt(root, "CannyThresholdLinking").value_or(setting.canny_threshold_linking);
    setting.line_min_length = GetInt(root, "LineMinLength").value_or(setting.line_min_length);
    setting.point_merge_delta = GetInt(root, "PointMergeDelta").value_or(setting.point_merge_delta);
    setting.epsilon = GetDouble(root, "Epsilon").value_or(setting.epsilon);
    return setting;
}

SettingTriangleDetection ReadSettingTriangleDetection(const JsonValue& root)
{
    SettingTriangleDetection setting;
    setting.min_area = GetInt(root, "MinArea").value_or(setting.min_area);
    setting.max_area = GetInt(root, "MaxArea").value_or(setting.max_area);
    setting.min_height = GetInt(root, "MinHeight").value_or(setting.min_height);
    setting.max_height = GetInt(root, "MaxHeight").value_or(setting.max_height);
    setting.min_width = GetInt(root, "MinWidth").value_or(setting.min_width);
    setting.max_width = GetInt(root, "MaxWidth").value_or(setting.max_width);
    setting.min_longest_length = GetInt(root, "minLongestLength").value_or(setting.min_longest_length);
    setting.max_longest_length = GetInt(root, "maxLongestLength").value_or(setting.max_longest_length);
    setting.in_range_lower = GetInt(root, "InRangeLower").value_or(setting.in_range_lower);
    setting.in_range_upper = GetInt(root, "InRangeUpper").value_or(setting.in_range_upper);
    return setting;
}

SettingTextBlock ReadSettingTextBlock(const JsonValue& root)
{
    SettingTextBlock setting;
    setting.perform_pyr_down = GetBool(root, "performPyrDown").value_or(setting.perform_pyr_down);
    setting.morphology_x = GetInt(root, "morphology_x").value_or(setting.morphology_x);
    setting.morphology_y = GetInt(root, "morphology_y").value_or(setting.morphology_y);
    setting.perform_second_morphology = GetBool(root, "performSecondMorphology").value_or(setting.perform_second_morphology);
    setting.morphology2_x = GetInt(root, "morphology2_x").value_or(setting.morphology2_x);
    setting.morphology2_y = GetInt(root, "morphology2_y").value_or(setting.morphology2_y);
    setting.ocr_language = GetString(root, "ocrLanguage").value_or(setting.ocr_language);
    setting.canny_threshold1 = GetInt(root, "canny_threshold1").value_or(setting.canny_threshold1);
    setting.canny_threshold2 = GetInt(root, "canny_threshold2").value_or(setting.canny_threshold2);
    return setting;
}

SettingFormatSet ReadSettingFormatSet(const JsonValue& root)
{
    SettingFormatSet setting;
    setting.setting_find_icon = GetString(root, "SettingFindIcon").value_or(setting.setting_find_icon);
    setting.setting_line_detection = GetString(root, "SettingLineDetection").value_or(setting.setting_line_detection);
    setting.setting_rectangle_detection = GetString(root, "SettingRectangleDetection").value_or(setting.setting_rectangle_detection);
    setting.setting_text_block = GetString(root, "SettingTextBlock").value_or(setting.setting_text_block);
    setting.setting_triangle_detection = GetString(root, "SettingTriangleDetection").value_or(setting.setting_triangle_detection);
    return setting;
}

OneSqlDefinition ReadOneSqlDefinition(const JsonValue& root)
{
    OneSqlDefinition definition;
    definition.name = GetString(root, "Name").value_or(definition.name);
    definition.connection_string = GetString(root, "ConnectionString").value_or(definition.connection_string);
    definition.sql = GetString(root, "Sql").value_or(definition.sql);
    return definition;
}

SettingSqlblock ReadSettingSqlblock(const JsonValue& root)
{
    SettingSqlblock setting;
    if (const auto* connections = FindProperty(root, "ConnectionStrings"); connections != nullptr) {
        if (const auto* object = connections->AsObject(); object != nullptr) {
            for (const auto& [key, value] : *object) {
                if (auto text = value.AsString(); text.has_value()) {
                    setting.connection_strings[key] = *text;
                }
            }
        }
    }
    if (const auto* sqls = FindProperty(root, "Sqls"); sqls != nullptr) {
        if (const auto* array = sqls->AsArray(); array != nullptr) {
            for (const auto& item : *array) {
                setting.sqls.push_back(ReadOneSqlDefinition(item));
            }
        }
    }
    setting.ResolveConnectionStrings();
    return setting;
}

std::optional<std::any> ReadKnownSetting(const std::string& type, const JsonValue& root)
{
    const auto normalized = Normalize(type);
    if (normalized == Normalize("SettingCanny")) {
        return ReadSettingCanny(root);
    }
    if (normalized == Normalize("SettingThreshold")) {
        return ReadSettingThreshold(root);
    }
    if (normalized == Normalize("SettingLoadMatGray")) {
        return ReadSettingLoadMatGray(root);
    }
    if (normalized == Normalize("SettingFindIcon")) {
        return ReadSettingFindIcon(root);
    }
    if (normalized == Normalize("SettingLineDetection")) {
        return ReadSettingLineDetection(root);
    }
    if (normalized == Normalize("SettingRectangleDetection")) {
        return ReadSettingRectangleDetection(root);
    }
    if (normalized == Normalize("SettingTriangleDetection")) {
        return ReadSettingTriangleDetection(root);
    }
    if (normalized == Normalize("SettingTextBlock")) {
        return ReadSettingTextBlock(root);
    }
    if (normalized == Normalize("SettingFormatSet")) {
        return ReadSettingFormatSet(root);
    }
    if (normalized == Normalize("SettingSqlblock")) {
        return ReadSettingSqlblock(root);
    }
    return std::nullopt;
}

std::optional<std::string> ReadMetaType(const JsonValue& root)
{
    const auto* meta = FindProperty(root, "__meta");
    if (meta == nullptr) {
        return std::nullopt;
    }
    return GetString(*meta, "type");
}

} // namespace

std::optional<std::pair<std::string, std::any>> LoadSetting::LoadSettingValueFromFile(const std::string& filename)
{
    std::string json;
    try {
        json = FileHelper::FileReadAllTextWithLnk(filename);
        const auto root = automationtest::utilities::reflection::ParseJson(json);
        if (!root.has_value()) {
            return std::nullopt;
        }

        const auto type = ReadMetaType(*root);
        if (!type.has_value() || type->empty()) {
            return std::nullopt;
        }

        const auto parsed = ReadKnownSetting(*type, *root);
        if (!parsed.has_value()) {
            return std::nullopt;
        }

        auto& load_functions = automationtest::utilities::status::LoadFunctions::Instance();
        const auto registered_type = load_functions.GetSettingType(*type).value_or(*type);
        return std::make_pair(registered_type, parsed.value());
    } catch (const std::exception& exception) {
        std::cerr << "when loading " << filename << " " << exception.what() << " " << json << '\n';
    }
    return std::nullopt;
}

void LoadSetting::LoadingSettingFromFile(const std::string& filename)
{
    const auto setting = LoadSettingValueFromFile(filename);
    if (!setting.has_value()) {
        return;
    }

    automationtest::utilities::services::VariableService::Instance().SaveSetting(setting->first, setting->second);
}

} // namespace automationtest::utilities::settings
