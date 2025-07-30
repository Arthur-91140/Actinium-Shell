// src/include/config.h
#pragma once
#include <string>

// Configuration constants
namespace Config {
    extern const std::string HISTORY_FILENAME;
    extern const int MAX_HISTORY_SIZE;
    extern const int MAX_SUGGESTIONS;
    extern const int FUZZY_MATCH_THRESHOLD;
    
    // Colors
    extern const int COLOR_NORMAL;
    extern const int COLOR_INPUT;
    extern const int COLOR_SUGGESTION;
    extern const int COLOR_PROMPT;
}

std::string getExecutableDirectory();
std::string getHistoryFilePath();