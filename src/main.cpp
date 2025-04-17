// Ecrit avec amour par Arthur Pruvost Rivière ^_.\^

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

    // Initialiser le répertoire courant
    char buffer[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, buffer);
    currentDirectory = buffer;

    std::string input;
    
    banner();   // Display the banner
    std::cout << "Commandes disponibles : hello [nom], ls [chemin], cd [chemin], pwd, exit\n";

    while (true) {
        std::cout << currentDirectory << "> ";  // Afficher le répertoire courant dans le prompt
        std::getline(std::cin, input);

        if (!input.empty()) {
            processCommand(input);
        }
    }

    return 0;
}