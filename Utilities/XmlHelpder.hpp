#ifndef AUTOMATIOTEST_XMLHELPDER_HPP
#define AUTOMATIOTEST_XMLHELPDER_HPP

#include <optional>
#include <string>

namespace automationtest::utilities {

class XmlHelpder {
public:
    static std::optional<std::string> ReadOneXmlFileAsString(const std::string& xml_filename);
    static std::optional<std::string> XDocumentFindFirstTagGetAttribute(const std::string& xml_content, const std::string& tag_name, const std::string& attribute_name);
    static std::optional<std::string> XDocumentGetFirstTagValue(const std::string& xml_content, const std::string& tag_name);
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_XMLHELPDER_HPP
