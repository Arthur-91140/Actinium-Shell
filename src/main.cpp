// main.cpp
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


int main() {
    SetConsoleOutputCP(65001); // Set UTF-8 code page for Windows console

    // Initialize the command system
    initializeCommandSystem();

    // Initialize the current directory
    char buffer[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, buffer);
    currentDirectory = buffer;

    std::string input;
    
    banner();   // Display the banner

    while (true) {
        std::cout << "AC " << currentDirectory << "> ";  // Display current directory in prompt
        std::getline(std::cin, input);

        if (!input.empty()) {
            processCommand(input);
        }
    }

    return 0;
}