// src/include/history-manager.h
#pragma once
#include <string>
#include <vector>
#include <map>

class HistoryManager {
private:
    std::vector<std::string> commandHistory;
    std::map<std::string, int> commandFrequency;
    std::string historyFilePath;
    
    void loadHistory();
    void saveHistory();
    
public:
    HistoryManager();
    ~HistoryManager();
    
    void addCommand(const std::string& command);
    std::vector<std::string> searchHistory(const std::string& query);
    std::vector<std::string> getRecentCommands(int count = 10);
    std::vector<std::string> getMostUsedCommands(int count = 10);
    std::vector<std::string> fuzzySearch(const std::string& query);
};