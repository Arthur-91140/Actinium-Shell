// src/auto-complete.cpp
#include "./include/auto-complete.h"
#include "./include/command-processor.h"
#include "./include/config.h"
#include <windows.h>
#include <algorithm>
#include <sstream>
#include <set>

AutoComplete::AutoComplete() {
    // Initialize built-in commands
    builtinCommands = {
        "hello", "exit", "pwd", "cd", "help", "version", "SPM-Install",
        "create", "edit", "compile", "reload", "ls", "dir", "clear", "cls"
    };
}

std::vector<std::string> AutoComplete::getDirectoryContents(const std::string& path) {
    std::vector<std::string> contents;
    std::string searchPath = path.empty() ? "*" : path + "\\*";
    
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
    
    if (hFind == INVALID_HANDLE_VALUE) {
        return contents;
    }
    
    do {
        std::string fileName = findData.cFileName;
        if (fileName != "." && fileName != "..") {
            contents.push_back(fileName);
        }
    } while (FindNextFileA(hFind, &findData));
    
    FindClose(hFind);
    return contents;
}

std::vector<std::string> AutoComplete::getExecutablesInPath() {
    std::vector<std::string> executables;
    
    char pathBuffer[32767];
    DWORD pathSize = GetEnvironmentVariableA("PATH", pathBuffer, sizeof(pathBuffer));
    
    if (pathSize == 0 || pathSize >= sizeof(pathBuffer)) {
        return executables;
    }
    
    std::string path(pathBuffer);
    std::istringstream pathStream(path);
    std::string pathDir;
    std::set<std::string> uniqueExes; // To avoid duplicates
    
    while (std::getline(pathStream, pathDir, ';')) {
        if (pathDir.empty()) continue;
        
        std::string searchPath = pathDir + "\\*.exe";
        WIN32_FIND_DATAA findData;
        HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
        
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                std::string fileName = findData.cFileName;
                // Remove .exe extension
                if (fileName.length() > 4 && fileName.substr(fileName.length() - 4) == ".exe") {
                    fileName = fileName.substr(0, fileName.length() - 4);
                }
                uniqueExes.insert(fileName);
            } while (FindNextFileA(hFind, &findData));
            
            FindClose(hFind);
        }
    }
    
    // Convert set to vector
    for (const auto& exe : uniqueExes) {
        executables.push_back(exe);
    }
    
    return executables;
}

int AutoComplete::levenshteinDistance(const std::string& s1, const std::string& s2) {
    int len1 = s1.length();
    int len2 = s2.length();
    
    std::vector<std::vector<int>> dp(len1 + 1, std::vector<int>(len2 + 1));
    
    for (int i = 0; i <= len1; ++i) dp[i][0] = i;
    for (int j = 0; j <= len2; ++j) dp[0][j] = j;
    
    for (int i = 1; i <= len1; ++i) {
        for (int j = 1; j <= len2; ++j) {
            if (s1[i-1] == s2[j-1]) {
                dp[i][j] = dp[i-1][j-1];
            } else {
                dp[i][j] = 1 + std::min({dp[i-1][j], dp[i][j-1], dp[i-1][j-1]});
            }
        }
    }
    
    return dp[len1][len2];
}

std::vector<std::string> AutoComplete::getCompletions(const std::string& input) {
    std::vector<std::string> completions;
    
    if (input.empty()) {
        return completions;
    }
    
    std::istringstream iss(input);
    std::string firstToken;
    iss >> firstToken;
    
    // If we're completing the first word (command)
    if (input.find(' ') == std::string::npos || input.back() != ' ') {
        // Get command completions
        std::vector<std::string> commandCompletions = getCommandSuggestions(firstToken);
        completions.insert(completions.end(), commandCompletions.begin(), commandCompletions.end());
    } else {
        // We're completing arguments - provide file completions
        std::string remainder;
        std::getline(iss, remainder);
        if (!remainder.empty() && remainder[0] == ' ') {
            remainder = remainder.substr(1); // Remove leading space
        }
        
        std::vector<std::string> fileCompletions = getFileCompletions(remainder);
        completions.insert(completions.end(), fileCompletions.begin(), fileCompletions.end());
    }
    
    return completions;
}

std::vector<std::string> AutoComplete::getFileCompletions(const std::string& partialPath) {
    std::vector<std::string> completions;
    
    std::string searchDir = ".";
    std::string prefix = partialPath;
    std::string pathPrefix = ""; // Pour conserver .\ ou ./ ou \ ou / ou espace
    
    // Gérer les préfixes spéciaux (y compris l'espace qui remet au répertoire courant)
    if (partialPath.length() >= 2 && partialPath.substr(0, 2) == ".\\") {
        pathPrefix = ".\\";
        prefix = partialPath.substr(2);
        searchDir = ".";
    } else if (partialPath.length() >= 2 && partialPath.substr(0, 2) == "./") {
        pathPrefix = "./";
        prefix = partialPath.substr(2);
        searchDir = ".";
    } else if (partialPath.length() >= 1 && partialPath[0] == '\\') {
        pathPrefix = "\\";
        prefix = partialPath.substr(1);
        searchDir = "\\";
    } else if (partialPath.length() >= 1 && partialPath[0] == '/') {
        pathPrefix = "/";
        prefix = partialPath.substr(1);
        searchDir = "\\"; // Convertir en Windows path
    } else {
        // Parse normal path pour séparer répertoire et nom de fichier
        size_t lastSlash = partialPath.find_last_of("\\/");
        if (lastSlash != std::string::npos) {
            searchDir = partialPath.substr(0, lastSlash);
            if (searchDir.empty()) searchDir = "\\"; // Root directory
            prefix = partialPath.substr(lastSlash + 1);
            pathPrefix = partialPath.substr(0, lastSlash + 1);
        } else {
            // Pas de slash trouvé, on cherche dans le répertoire courant
            // Le partialPath est juste le début du nom de fichier
            searchDir = ".";
            prefix = partialPath;
            pathPrefix = "";
        }
    }
    
    // Handle special cases
    if (partialPath.empty()) {
        searchDir = ".";
        prefix = "";
        pathPrefix = "";
    }
    
    std::vector<std::string> contents = getDirectoryContents(searchDir);
    
    for (const auto& item : contents) {
        if (prefix.empty() || (item.length() >= prefix.length() && 
            item.substr(0, prefix.length()) == prefix)) {
            
            std::string completion = pathPrefix + item;
            
            // Check if it's a directory and add trailing slash
            std::string fullItemPath;
            if (searchDir == ".") {
                fullItemPath = item;
            } else if (searchDir == "\\") {
                fullItemPath = "\\" + item;
            } else {
                fullItemPath = searchDir + "\\" + item;
            }
            
            DWORD attrs = GetFileAttributesA(fullItemPath.c_str());
            if (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY)) {
                completion += "\\";
            }
            
            completions.push_back(completion);
        }
    }
    
    return completions;
}

std::vector<std::string> AutoComplete::getCommandSuggestions(const std::string& input) {
    std::vector<std::string> suggestions;
    
    if (input.empty()) {
        return suggestions;
    }
    
    // Convert input to lowercase for case-insensitive matching
    std::string lowerInput = input;
    std::transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::tolower);
    
    // Check built-in commands
    for (const auto& cmd : builtinCommands) {
        std::string lowerCmd = cmd;
        std::transform(lowerCmd.begin(), lowerCmd.end(), lowerCmd.begin(), ::tolower);
        
        if (lowerCmd.find(lowerInput) == 0) { // Prefix match
            suggestions.push_back(cmd);
        }
    }
    
    // Check loaded dynamic commands
    for (const auto& pair : loadedCommands) {
        std::string lowerCmd = pair.first;
        std::transform(lowerCmd.begin(), lowerCmd.end(), lowerCmd.begin(), ::tolower);
        
        if (lowerCmd.find(lowerInput) == 0) { // Prefix match
            suggestions.push_back(pair.first);
        }
    }
    
    // Add fuzzy matches from executables if we don't have enough suggestions
    if (suggestions.size() < 3) {
        std::vector<std::string> executables = getExecutablesInPath();
        std::vector<std::string> fuzzyMatches = fuzzyMatch(input, executables);
        
        // Add fuzzy matches that aren't already in suggestions
        for (const auto& match : fuzzyMatches) {
            if (std::find(suggestions.begin(), suggestions.end(), match) == suggestions.end()) {
                suggestions.push_back(match);
                if (suggestions.size() >= Config::MAX_SUGGESTIONS) break;
            }
        }
    }
    
    return suggestions;
}

std::vector<std::string> AutoComplete::fuzzyMatch(const std::string& input, const std::vector<std::string>& candidates) {
    std::vector<std::pair<std::string, int>> scored;
    
    for (const auto& candidate : candidates) {
        int score = 0;
        std::string lowerCandidate = candidate;
        std::string lowerInput = input;
        
        std::transform(lowerCandidate.begin(), lowerCandidate.end(), lowerCandidate.begin(), ::tolower);
        std::transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::tolower);
        
        // Prefix match gets highest score
        if (lowerCandidate.find(lowerInput) == 0) {
            score += 1000;
        }
        // Contains input gets medium score
        else if (lowerCandidate.find(lowerInput) != std::string::npos) {
            score += 500;
        }
        // Fuzzy matching - characters appear in order
        else {
            size_t inputPos = 0;
            for (size_t i = 0; i < lowerCandidate.length() && inputPos < lowerInput.length(); ++i) {
                if (lowerCandidate[i] == lowerInput[inputPos]) {
                    score += 10;
                    inputPos++;
                }
            }
            // Bonus if all characters found
            if (inputPos == lowerInput.length()) {
                score += 100;
            }
        }
        
        // Levenshtein distance bonus (closer = higher score)
        int distance = levenshteinDistance(lowerInput, lowerCandidate);
        if (distance <= 3) { // Only consider close matches
            score += (4 - distance) * 50;
        }
        
        if (score > Config::FUZZY_MATCH_THRESHOLD / 2) {
            scored.push_back({candidate, score});
        }
    }
    
    // Sort by score (descending)
    std::sort(scored.begin(), scored.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<std::string> results;
    for (const auto& pair : scored) {
        results.push_back(pair.first);
        if (results.size() >= Config::MAX_SUGGESTIONS) break;
    }
    
    return results;
}