#include "ListOfMatHelper.hpp"

#include "OpenCvLib.hpp"

#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace automationtest::opencvlib {

namespace {

bool ExtensionMatches(const std::filesystem::path& path, const std::string& extension)
{
    if (extension.empty() || extension == "*.*" || extension == "*") {
        return true;
    }

    if (extension.size() > 2 && extension[0] == '*' && extension[1] == '.') {
        return path.extension().string() == extension.substr(1);
    }

    return path.extension().string() == extension;
}

cv::Mat LoadPortableGraymap(const std::filesystem::path& file)
{
    std::ifstream input(file, std::ios::binary);
    if (!input) {
        return {};
    }

    std::string magic;
    input >> magic;
    if (magic != "P5") {
        return {};
    }

    auto skip_comments = [&input]() {
        while (std::isspace(input.peek())) {
            input.get();
        }
        while (input.peek() == '#') {
            std::string ignored;
            std::getline(input, ignored);
            while (std::isspace(input.peek())) {
                input.get();
            }
        }
    };

    skip_comments();
    int width = 0;
    int height = 0;
    int max_value = 255;
    input >> width;
    skip_comments();
    input >> height;
    skip_comments();
    input >> max_value;
    input.get();

    if (width <= 0 || height <= 0 || max_value <= 0) {
        return {};
    }

    cv::Mat mat(height, width, CV_8UC1);
    input.read(reinterpret_cast<char*>(mat.data), static_cast<std::streamsize>(mat.total() * mat.elemSize()));
    if (!input) {
        return {};
    }

    return mat;
}

} // namespace

ListOfMat ListOfMatHelper::LoadListOfImageAsGrayMat(const std::string& folder, const automationtest::utilities::settings::SettingLoadMatGray* mat_gray, const std::string& extension)
{
    ListOfMat result {};
    if (folder.empty() || !std::filesystem::exists(folder)) {
        return result;
    }

    for (const auto& entry : std::filesystem::directory_iterator(folder)) {
        if (!entry.is_regular_file() || !ExtensionMatches(entry.path(), extension)) {
            continue;
        }

        auto mat = LoadPortableGraymap(entry.path());
        if (mat.empty()) {
            continue;
        }

        if (mat_gray != nullptr) {
            mat = OpenCvLib::ApplyThresholdReturnRawIfFailed(mat, *mat_gray);
        }
        result.mats.push_back(std::move(mat));
    }

    return result;
}

} // namespace automationtest::opencvlib
