#include "OSRelatedFunctions.hpp"
#include "../GlobalSetting.hpp"
#include "../Logger.hpp"
#include "../Types/AnyCast.hpp"

void OSRelatedFunctions::RegisterBindings(automationtest::utilities::status::LoadFunctions& load_functions)
{
    using Parameter = automationtest::utilities::status::LoadFunctions::RegisteredParameter;

    load_functions.RegisterMethod("IsBrowserVisible", automationtest::utilities::status::LoadFunctions::RegisteredMethod{
        .declaring_type = "IsBrowserVisible",
        .invoke = [](const std::vector<std::any>& list) -> std::any {
            return IsBrowserVisible(automationtest::utilities::types::AnyCast::CastOne<std::unordered_set<std::wstring>*>(list, "IsBrowserVisible"));
        },
        .parameters = {
            Parameter {.name = "browsers"},
            },
        });
}


#if defined(_WIN32)
    std::string ShellQuote(const std::string& value)
    {
        std::string result{ "\"" };
        for (const char ch : value) {
            if (ch == '"') {
                result += "\\\"";
            }
            else {
                result += ch;
            }
        }
        result += '"';
        return result;
    }
#else
    std::string ShellQuote(const std::string& value)
    {
        std::string result{ "'" };
        for (const char ch : value) {
            if (ch == '\'') {
                result += "'\\''";
            }
            else {
                result += ch;
            }
        }
        result += '\'';
        return result;
    }
#endif


    void StartBitmapViewer(const std::string& filename, const std::string& log_category)
    {
        const auto tool = automationtest::utilities::GlobalSetting::ToolsToViewBitmap();
        std::string command;

#if defined(_WIN32)
        if (tool.empty()) {
            command = "start \"\" " + ShellQuote(filename);
        }
        else {
            command = "start \"\" " + ShellQuote(tool) + " " + ShellQuote(filename);
        }
#elif defined(__APPLE__)
        if (tool.empty()) {
            command = "open " + ShellQuote(filename) + " >/dev/null 2>&1 &";
        }
        else {
            command = ShellQuote(tool) + " " + ShellQuote(filename) + " >/dev/null 2>&1 &";
        }
#else
        if (tool.empty()) {
            command = "xdg-open " + ShellQuote(filename) + " >/dev/null 2>&1 &";
        }
        else {
            command = ShellQuote(tool) + " " + ShellQuote(filename) + " >/dev/null 2>&1 &";
        }
#endif

        const int result = std::system(command.c_str());
        if (result != 0) {
            automationtest::utilities::Logger::Error("Failed to start bitmap viewer for " + filename, log_category);
        }
    }

