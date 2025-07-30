// src/include/auto-complete.h
#pragma once
#include <string>
#include <vector>

class AutoComplete {
private:
    std::vector<std::string> builtinCommands;
    
    std::vector<std::string> getDirectoryContents(const std::string& path);
    std::vector<std::string> getExecutablesInPath();
    int levenshteinDistance(const std::string& s1, const std::string& s2);
    
public:
    AutoComplete();
    
    std::vector<std::string> getCompletions(const std::string& input);
    std::vector<std::string> getFileCompletions(const std::string& partialPath);
    std::vector<std::string> getCommandSuggestions(const std::string& input);
    std::vector<std::string> fuzzyMatch(const std::string& input, const std::vector<std::string>& candidates);
};