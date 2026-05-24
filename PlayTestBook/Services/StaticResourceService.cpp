#include "StaticResourceService.hpp"

#include "../../TestBookLib/Parser/TestBookParser.hpp"
#include "../../Utilities/CommonTypes.hpp"
#include "../../Utilities/FilePathLib.hpp"
#include "../../Utilities/StringLib.hpp"
#include "../../Utilities/Status/LoadFunctions.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace automationtest::playtestbook::services {

namespace {

utilities::Bitmap LoadBitmapPlaceholder(const std::string& filename)
{
    std::ifstream input(filename, std::ios::binary);
    std::vector<char> raw((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());

    utilities::Bitmap bitmap {};
    bitmap.channels = 1;
    bitmap.stride = static_cast<int>(raw.size());
    bitmap.width = bitmap.stride;
    bitmap.height = raw.empty() ? 0 : 1;
    bitmap.pixels.resize(raw.size());
    for (std::size_t index = 0; index < raw.size(); ++index) {
        bitmap.pixels[index] = static_cast<std::byte>(static_cast<unsigned char>(raw[index]));
    }
    return bitmap;
}

std::vector<std::string> SplitResourceLine(const std::string& line)
{
    std::istringstream input(line);
    std::vector<std::string> parts {};
    std::string part {};
    while (input >> part) {
        parts.push_back(part);
    }
    return parts;
}

} // namespace

StaticResourceService& StaticResourceService::Instance()
{
    static StaticResourceService instance;
    return instance;
}

int StaticResourceService::LoadObjects(const std::string& folder, utilities::services::VariableService& variable_service)
{
    const auto filename = std::filesystem::path(folder) / testbooklib::parser::TestBookParser::StaticResourceFileName;
    if (!std::filesystem::exists(filename)) {
        return 0;
    }

    std::ifstream input(filename);
    std::string line {};
    int loaded = 0;

    while (std::getline(input, line)) {
        if (utilities::StringLib::StartsWithAfterTrim(line, '#')) {
            continue;
        }
        const auto parts = SplitResourceLine(line);
        if (parts.size() < 3) {
            continue;
        }

        const auto full_filename = utilities::FilePathLib::AddParentPath(parts[2], folder);
        if (utilities::status::CaseInsensitiveEqual {}(parts[0], "bitmap")) {
            variable_service.SetObject(parts[1], LoadBitmapPlaceholder(full_filename));
            ++loaded;
            continue;
        }

        const auto loader = utilities::status::LoadFunctions::Instance().GetSettingLoader(parts[0]);
        if (loader.has_value()) {
            variable_service.SetObject(parts[1], loader.value()(full_filename));
            ++loaded;
            continue;
        }

        std::ifstream text_input(full_filename, std::ios::binary);
        std::string content((std::istreambuf_iterator<char>(text_input)), std::istreambuf_iterator<char>());
        variable_service.SetObject(parts[1], content);
        ++loaded;
    }

    return loaded;
}

} // namespace automationtest::playtestbook::services
