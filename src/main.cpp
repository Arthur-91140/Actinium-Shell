// src/main.cpp
// Written with love by Arthur Pruvost Rivière ^^

#include <iostream>
#include <string>
#include <sstream>
#include <locale>
#include <codecvt>
#include <cstdlib>
#include <windows.h>
#include "./include/command-processor.h"
#include "./include/init.h"
#include "./include/input-handler.h"
#include "./include/history-manager.h"
#include "./include/auto-complete.h"
#include "./include/config.h"

// Global instances
HistoryManager* g_historyManager = nullptr;
AutoComplete* g_autoComplete = nullptr;
InputHandler* g_inputHandler = nullptr;

// Function to cleanup and exit
void cleanupAndExit() {
    std::cout << "Fermeture du terminal. \n";
    
    // Force save history before exit
    if (g_historyManager) {
        delete g_historyManager; // This will call the destructor and save history
    }
    if (g_autoComplete) {
        delete g_autoComplete;
    }
    if (g_inputHandler) {
        delete g_inputHandler;
    }
    
    exit(0);
}

int main() {
    SetConsoleOutputCP(65001); // Set UTF-8 code page for Windows console

    // Initialize global instances
    g_historyManager = new HistoryManager();
    g_autoComplete = new AutoComplete();
    g_inputHandler = new InputHandler();

    // Initialize the command system
    initializeCommandSystem();

    // Initialize the current directory
    char buffer[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, buffer);
    currentDirectory = buffer;

    std::string input;
    
    banner();   // Display the banner
    
    std::cout << "Enhanced AC Shell - Type 'help' for available commands\n";
    std::cout << "Features: Fuzzy search, auto-completion (Tab), command history (Up/Down)\n";
    std::cout << "History file: " << getHistoryFilePath() << "\n\n";

    while (true) {
        input = g_inputHandler->readInput();

        if (!input.empty()) {
            // Check for exit command here to ensure proper cleanup
            if (input == "exit") {
                cleanupAndExit();
            }
            processCommand(input);
        }
    }

    return 0;
}