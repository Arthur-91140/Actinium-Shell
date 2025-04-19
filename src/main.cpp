// main.cpp
// Written with love by Arthur Pruvost Rivière ^_.\^

#include <iostream>
#include <string>
#include <sstream>
#include <locale>
#include <codecvt>
#include <cstdlib>
#include <windows.h>
#include "./include/command-processor.h"
#include "./include/init.h"
#include "./include/installer.h"


int main() {
    SetConsoleOutputCP(65001); // Set UTF-8 code page for Windows console

    run_main(); // Run the installer

    // Initialize the command system
    initializeCommandSystem();

    // Initialize the current directory
    char buffer[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, buffer);
    currentDirectory = buffer;

    std::string input;
    
    banner();   // Display the banner
    std::cout << "Welcome to ACTINIUM Shell - Type 'help' for available commands\n";

    while (true) {
        std::cout << currentDirectory << "> ";  // Display current directory in prompt
        std::getline(std::cin, input);

        if (!input.empty()) {
            processCommand(input);
        }
    }

    return 0;
}