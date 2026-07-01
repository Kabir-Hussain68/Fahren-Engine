#include "fhpch.h"
#include "Engine/utils/platformUtils.h"

#include <array>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <sstream>

std::string FileDialogs::openFile(const char* filter)
{
    std::array<char, 1024> buffer{};
    std::string result;

    std::filesystem::path currentDir = std::filesystem::current_path();

    std::stringstream cmd;
    cmd << "zenity --file-selection "
        << "--title=\"Open File\" "
        << "--filename=\"" << currentDir.string() << "/\" ";

    // Convert Win32 filter to Zenity filter
    if (filter)
    {
        const char* description = filter;
        const char* pattern = strchr(filter, '\0');

        if (pattern && *(++pattern))
        {
            cmd << "--file-filter=\""
                << description << " | "
                << pattern
                << "\" ";
        }
    }

    FILE* pipe = popen(cmd.str().c_str(), "r");
    if (!pipe)
        return "";

    if (fgets(buffer.data(), buffer.size(), pipe))
        result = buffer.data();

    int status = pclose(pipe);

    if (status != 0)
        return "";

    if (!result.empty() && result.back() == '\n')
        result.pop_back();

    return result;
}

std::string FileDialogs::saveFile(const char* filter)
{
    std::array<char, 1024> buffer{};
    std::string result;

    std::filesystem::path currentDir = std::filesystem::current_path();

    std::stringstream cmd;
    cmd << "zenity --file-selection "
        << "--save "
        << "--confirm-overwrite "
        << "--title=\"Save File\" "
        << "--filename=\"" << currentDir.string() << "/\" ";

    if (filter)
    {
        const char* pattern = strchr(filter, '\0');
        if (pattern && *(++pattern))
            cmd << "--file-filter=\"" << pattern << "\" ";
    }

    FILE* pipe = popen(cmd.str().c_str(), "r");
    if (!pipe)
        return "";

    if (fgets(buffer.data(), buffer.size(), pipe))
        result = buffer.data();

    int status = pclose(pipe);

    if (status != 0)
        return "";

    if (!result.empty() && result.back() == '\n')
        result.pop_back();

    // Append default extension if missing
    if (!result.empty() && filter)
    {
        const char* pattern = strchr(filter, '\0');
        if (pattern && *(++pattern))
        {
            const char* dot = strchr(pattern, '.');
            if (dot)
            {
                std::string extension(dot);

                // Trim anything after the extension (e.g. "*.fahren *.scene")
                size_t end = extension.find_first_of(" ;");
                if (end != std::string::npos)
                    extension = extension.substr(0, end);

                if (result.size() < extension.size() ||
                    result.substr(result.size() - extension.size()) != extension)
                {
                    result += extension;
                }
            }
        }
    }

    return result;
}