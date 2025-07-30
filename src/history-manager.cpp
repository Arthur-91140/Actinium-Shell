// src/history-manager.cpp (Version améliorée)
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
    commandHistory.clear();
    commandFrequency.clear();
    
    while (std::getline(file, line)) {
        if (!line.empty()) {
            // Format: command|frequency
            size_t pos = line.find('|');
            if (pos != std::string::npos) {
                std::string command = line.substr(0, pos);
                int frequency = std::stoi(line.substr(pos + 1));
                
                // Vérifier si la commande existe déjà
                auto it = std::find(commandHistory.begin(), commandHistory.end(), command);
                if (it == commandHistory.end()) {
                    commandHistory.push_back(command);
                    commandFrequency[command] = frequency;
                } else {
                    // Si elle existe déjà, prendre la fréquence la plus élevée
                    commandFrequency[command] = std::max(commandFrequency[command], frequency);
                }
            } else {
                // Legacy format without frequency
                auto it = std::find(commandHistory.begin(), commandHistory.end(), line);
                if (it == commandHistory.end()) {
                    commandHistory.push_back(line);
                    commandFrequency[line] = 1;
                } else {
                    commandFrequency[line]++;
                }
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
    
    // Créer un ensemble des commandes uniques avec leur fréquence
    std::set<std::string> uniqueCommands;
    
    // Sauvegarder les commandes récentes avec leur fréquence (sans doublons)
    size_t startIndex = commandHistory.size() > Config::MAX_HISTORY_SIZE ? 
                       commandHistory.size() - Config::MAX_HISTORY_SIZE : 0;
    
    // D'abord, collecter toutes les commandes uniques
    for (size_t i = startIndex; i < commandHistory.size(); ++i) {
        uniqueCommands.insert(commandHistory[i]);
    }
    
    // Ensuite, les écrire avec leur fréquence
    for (const auto& command : uniqueCommands) {
        file << command << "|" << commandFrequency[command] << std::endl;
    }
    
    file.close();
}

void HistoryManager::addCommand(const std::string& command) {
    if (command.empty()) return;
    
    // Chercher si la commande existe déjà dans l'historique
    auto it = std::find(commandHistory.begin(), commandHistory.end(), command);
    
    if (it != commandHistory.end()) {
        // La commande existe déjà
        // L'enlever de sa position actuelle pour la remettre à la fin
        commandHistory.erase(it);
        // Incrémenter sa fréquence
        commandFrequency[command]++;
    } else {
        // Nouvelle commande
        commandFrequency[command] = 1;
    }
    
    // Ajouter la commande à la fin de l'historique
    commandHistory.push_back(command);
    
    // Limiter la taille de l'historique
    while (commandHistory.size() > Config::MAX_HISTORY_SIZE) {
        std::string removedCommand = commandHistory.front();
        commandHistory.erase(commandHistory.begin());
        
        // Si cette commande n'apparaît plus dans l'historique, supprimer sa fréquence
        if (std::find(commandHistory.begin(), commandHistory.end(), removedCommand) == commandHistory.end()) {
            commandFrequency.erase(removedCommand);
        }
    }
    
    // Sauvegarder immédiatement pour éviter la perte de données
    saveHistory();
}

std::vector<std::string> HistoryManager::searchHistory(const std::string& query) {
    std::vector<std::string> results;
    std::set<std::string> uniqueResults; // Pour éviter les doublons
    
    for (const auto& command : commandHistory) {
        if (command.find(query) != std::string::npos) {
            // Éviter les doublons
            if (uniqueResults.find(command) == uniqueResults.end()) {
                results.push_back(command);
                uniqueResults.insert(command);
                
                // Limiter le nombre de résultats
                if (results.size() >= Config::MAX_SUGGESTIONS) {
                    break;
                }
            }
        }
    }
    
    return results;
}

std::vector<std::string> HistoryManager::getRecentCommands(int count) {
    std::vector<std::string> recent;
    std::set<std::string> uniqueCommands; // Pour éviter les doublons
    
    // Parcourir l'historique en sens inverse pour avoir les plus récentes en premier
    for (int i = commandHistory.size() - 1; i >= 0 && recent.size() < count; --i) {
        const std::string& command = commandHistory[i];
        if (uniqueCommands.find(command) == uniqueCommands.end()) {
            recent.push_back(command);
            uniqueCommands.insert(command);
        }
    }
    
    return recent;
}

std::vector<std::string> HistoryManager::getMostUsedCommands(int count) {
    std::vector<std::pair<std::string, int>> frequencyPairs;
    
    for (const auto& pair : commandFrequency) {
        frequencyPairs.push_back(pair);
    }
    
    // Trier par fréquence (décroissant)
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
    std::set<std::string> processedCommands; // Pour éviter les doublons
    
    for (const auto& command : commandHistory) {
        // Éviter de traiter la même commande plusieurs fois
        if (processedCommands.find(command) != processedCommands.end()) {
            continue;
        }
        processedCommands.insert(command);
        
        int score = 0;
        
        // Correspondance exacte au début obtient le score le plus élevé
        if (command.find(query) == 0) {
            score += 1000;
        }
        // Contient la requête obtient un score moyen
        else if (command.find(query) != std::string::npos) {
            score += 500;
        }
        // Correspondance floue basée sur la proximité des caractères
        else {
            int fuzzyScore = 0;
            size_t queryPos = 0;
            for (size_t i = 0; i < command.length() && queryPos < query.length(); ++i) {
                if (std::tolower(command[i]) == std::tolower(query[queryPos])) {
                    fuzzyScore += 10;
                    queryPos++;
                }
            }
            if (queryPos == query.length()) { // Tous les caractères de la requête trouvés
                score += fuzzyScore;
            }
        }
        
        // Bonus basé sur la fréquence
        score += commandFrequency.at(command) * 10;
        
        if (score > Config::FUZZY_MATCH_THRESHOLD) {
            candidates.push_back({command, score});
        }
    }
    
    // Trier par score (décroissant)
    std::sort(candidates.begin(), candidates.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Retourner les meilleurs résultats (sans doublons garantis par le set)
    std::vector<std::string> results;
    for (const auto& candidate : candidates) {
        results.push_back(candidate.first);
        if (results.size() >= Config::MAX_SUGGESTIONS) {
            break;
        }
    }
    
    return results;
}