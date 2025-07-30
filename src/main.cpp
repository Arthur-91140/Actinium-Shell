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

    while (true) {
        input = g_inputHandler->readInput();

        if (!input.empty()) {
            processCommand(input);
        }
    }

    // Clean up (though we never reach here in normal execution)
    delete g_historyManager;
    delete g_autoComplete;
    delete g_inputHandler;

    return 0;
}