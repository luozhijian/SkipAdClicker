#ifndef AUTOMATIOTEST_FILEPATH_READFILEASVALUEPAIRLIST_HPP
#define AUTOMATIOTEST_FILEPATH_READFILEASVALUEPAIRLIST_HPP

#include <string>
#include <utility>
#include <vector>

namespace automationtest::utilities::file_path {

class ReadFileAsValuePairList {
public:
    static std::vector<std::pair<std::string, std::string>> Read(const std::string& file_path, const std::string& split);
};

} // namespace automationtest::utilities::file_path

#endif // AUTOMATIOTEST_FILEPATH_READFILEASVALUEPAIRLIST_HPP
