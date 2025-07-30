// src/history-manager.cpp
#include "./include/history-manager.h"
#include "./include/config.h"
#include <fstream>
#include <algorithm>
#include <sstream>
#include <set>

HistoryManager::HistoryManager() {
    historyFilePath = getHistoryFilePath();
    loadHistory();
}

HistoryManager::~HistoryManager() {
    saveHistory();
}

void HistoryManager::loadHistory() {
    std::ifstream file(historyFilePath);
    if (!file.is_open()) {
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            // Format: command|frequency
            size_t pos = line.find('|');
            if (pos != std::string::npos) {
                std::string command = line.substr(0, pos);
                int frequency = std::stoi(line.substr(pos + 1));
                
                commandHistory.push_back(command);
                commandFrequency[command] = frequency;
            } else {
                // Legacy format without frequency
                commandHistory.push_back(line);
                commandFrequency[line] = 1;
            }
        }
    }
    
    file.close();
}

void HistoryManager::saveHistory() {
    std::ofstream file(historyFilePath);
    if (!file.is_open()) {
        return;
    }
    
    // Save recent commands with frequency
    size_t startIndex = commandHistory.size() > Config::MAX_HISTORY_SIZE ? 
                       commandHistory.size() - Config::MAX_HISTORY_SIZE : 0;
    
    for (size_t i = startIndex; i < commandHistory.size(); ++i) {
        const std::string& command = commandHistory[i];
        file << command << "|" << commandFrequency[command] << std::endl;
    }
    
    file.close();
}

void HistoryManager::addCommand(const std::string& command) {
    if (command.empty()) return;
    
    // Update frequency
    commandFrequency[command]++;
    
    // Add to history (avoid consecutive duplicates)
    if (commandHistory.empty() || commandHistory.back() != command) {
        commandHistory.push_back(command);
    }
    
    // Limit history size
    if (commandHistory.size() > Config::MAX_HISTORY_SIZE) {
        commandHistory.erase(commandHistory.begin());
    }
}

std::vector<std::string> HistoryManager::searchHistory(const std::string& query) {
    std::vector<std::string> results;
    
    for (const auto& command : commandHistory) {
        if (command.find(query) != std::string::npos) {
            // Avoid duplicates
            if (std::find(results.begin(), results.end(), command) == results.end()) {
                results.push_back(command);
            }
        }
    }
    
    return results;
}

std::vector<std::string> HistoryManager::getRecentCommands(int count) {
    std::vector<std::string> recent;
    std::vector<std::string> uniqueCommands;
    
    // Get unique recent commands (reverse order to get most recent first)
    for (int i = commandHistory.size() - 1; i >= 0 && uniqueCommands.size() < count; --i) {
        const std::string& command = commandHistory[i];
        if (std::find(uniqueCommands.begin(), uniqueCommands.end(), command) == uniqueCommands.end()) {
            uniqueCommands.push_back(command);
        }
    }
    
    return uniqueCommands;
}

std::vector<std::string> HistoryManager::getMostUsedCommands(int count) {
    std::vector<std::pair<std::string, int>> frequencyPairs;
    
    for (const auto& pair : commandFrequency) {
        frequencyPairs.push_back(pair);
    }
    
    // Sort by frequency (descending)
    std::sort(frequencyPairs.begin(), frequencyPairs.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<std::string> mostUsed;
    for (int i = 0; i < std::min(count, (int)frequencyPairs.size()); ++i) {
        mostUsed.push_back(frequencyPairs[i].first);
    }
    
    return mostUsed;
}

std::vector<std::string> HistoryManager::fuzzySearch(const std::string& query) {
    if (query.empty()) {
        return getRecentCommands(Config::MAX_SUGGESTIONS);
    }
    
    std::vector<std::pair<std::string, int>> candidates;
    
    for (const auto& command : commandHistory) {
        int score = 0;
        
        // Exact prefix match gets highest score
        if (command.find(query) == 0) {
            score += 1000;
        }
        // Contains query gets medium score
        else if (command.find(query) != std::string::npos) {
            score += 500;
        }
        // Fuzzy matching based on character proximity
        else {
            int fuzzyScore = 0;
            size_t queryPos = 0;
            for (size_t i = 0; i < command.length() && queryPos < query.length(); ++i) {
                if (std::tolower(command[i]) == std::tolower(query[queryPos])) {
                    fuzzyScore += 10;
                    queryPos++;
                }
            }
            if (queryPos == query.length()) { // All query characters found
                score += fuzzyScore;
            }
        }
        
        // Boost score based on frequency
        score += commandFrequency[command] * 10;
        
        if (score > Config::FUZZY_MATCH_THRESHOLD) {
            candidates.push_back({command, score});
        }
    }
    
    // Sort by score (descending)
    std::sort(candidates.begin(), candidates.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Remove duplicates and return top results
    std::vector<std::string> results;
    std::set<std::string> seen;
    
    for (const auto& candidate : candidates) {
        if (seen.find(candidate.first) == seen.end() && results.size() < Config::MAX_SUGGESTIONS) {
            results.push_back(candidate.first);
            seen.insert(candidate.first);
        }
    }
    
    return results;
}