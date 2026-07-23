#pragma once
#include <string>
#include <unordered_set>

#include "../Status/LoadFunctions.hpp"

class OSRelatedFunctions {
public:
    static bool IsBrowserVisible(std::unordered_set<std::wstring>* browsers);

    static void RegisterBindings(automationtest::utilities::status::LoadFunctions& load_functions);
};

std::string ShellQuote(const std::string& value);
void StartBitmapViewer(const std::string& filename, const std::string& log_category);
