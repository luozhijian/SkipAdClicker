#ifndef AUTOMATIOTEST_OPENCVLIB_LISTOFMATHELPER_HPP
#define AUTOMATIOTEST_OPENCVLIB_LISTOFMATHELPER_HPP

#include "ListOfMat.hpp"
#include "../Utilities/Settings/SettingLoadMatGray.hpp"

#include <string>

namespace automationtest::opencvlib {

class ListOfMatHelper {
public:
    static ListOfMat LoadListOfImageAsGrayMat(const std::string& folder, const automationtest::utilities::settings::SettingLoadMatGray* mat_gray = nullptr, const std::string& extension = "*.pgm");
};

} // namespace automationtest::opencvlib

#endif // AUTOMATIOTEST_OPENCVLIB_LISTOFMATHELPER_HPP
