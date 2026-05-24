#include "XmlHelpder.hpp"

#include "FileHelper.hpp"

#include <regex>

namespace automationtest::utilities {

std::optional<std::string> XmlHelpder::ReadOneXmlFileAsString(const std::string& xml_filename)
{
    try {
        return FileHelper::FileReadAllText(xml_filename);
    } catch (...) {
        return std::nullopt;
    }
}

std::optional<std::string> XmlHelpder::XDocumentFindFirstTagGetAttribute(const std::string& xml_content, const std::string& tag_name, const std::string& attribute_name)
{
    const std::regex pattern("<" + tag_name + "[^>]*\\s" + attribute_name + "=\"([^\"]*)\"", std::regex_constants::icase);
    std::smatch match;
    if (std::regex_search(xml_content, match, pattern)) {
        return match[1].str();
    }
    return std::nullopt;
}

std::optional<std::string> XmlHelpder::XDocumentGetFirstTagValue(const std::string& xml_content, const std::string& tag_name)
{
    const std::regex pattern("<" + tag_name + R"([^>]*>(.*?)</)" + tag_name + ">", std::regex_constants::icase);
    std::smatch match;
    if (std::regex_search(xml_content, match, pattern)) {
        return match[1].str();
    }
    return std::nullopt;
}

} // namespace automationtest::utilities
