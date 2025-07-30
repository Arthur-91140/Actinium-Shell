// src/config.cpp
#include "./include/config.h"
#include <windows.h>
#include <filesystem>

namespace Config {
    const std::string HISTORY_FILENAME = "command_history.txt";
    const int MAX_HISTORY_SIZE = 1000;
    const int MAX_SUGGESTIONS = 5;
    const int FUZZY_MATCH_THRESHOLD = 50;
    
    // Colors matching the image exactly
    const int COLOR_NORMAL = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; // White
    const int COLOR_INPUT = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY; // Yellow (text being typed)
    const int COLOR_SUGGESTION = FOREGROUND_INTENSITY; // Dark Gray (suggestion completion)
    const int COLOR_PROMPT = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; // WHITE (prompt)
}

std::string getExecutableDirectory() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    
    std::string execPath(buffer);
    size_t pos = execPath.find_last_of("\\/");
    
    if (pos != std::string::npos) {
        return execPath.substr(0, pos);
    }
    
    return ".";
}

std::string getHistoryFilePath() {
    return getExecutableDirectory() + "\\" + Config::HISTORY_FILENAME;
}