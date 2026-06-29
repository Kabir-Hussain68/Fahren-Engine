#include "fhpch.h"
#include "Engine/utils/platformUtils.h"

#include <cstdio>
#include <array>

std::string FileDialogs::openFile(const char* filter)
{
    std::array<char, 256> buffer;
    std::string result;
    std::string cmd = "zenity --file-selection --title=\"Open File\"";
    
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "";
    
    if (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
        result = buffer.data();
    
    pclose(pipe);
    
    // Remove trailing newline
    if (!result.empty() && result.back() == '\n')
        result.pop_back();
    
    return result;
}

std::string FileDialogs::saveFile(const char* filter)
{
    std::array<char, 256> buffer;
    std::string result;
    std::string cmd = "zenity --file-selection --save --confirm-overwrite --title=\"Save File\"";
    
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "";
    
    if (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
        result = buffer.data();
    
    pclose(pipe);
    
    if (!result.empty() && result.back() == '\n')
        result.pop_back();
    
    return result;
}
