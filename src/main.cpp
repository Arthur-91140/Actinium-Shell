#include <iostream>
#include <string>
#include <sstream>
#include <locale>
#include <codecvt>
#include <cstdlib>
#include <windows.h>
#include "./include/command-processor.h"

int main() {
    SetConsoleOutputCP(65001); // Set UTF-8 code page for Windows console

    std::string input;
    
    std::cout << "Terminal personnalisé démarré. 🚀\n";
    std::cout << "Commandes disponibles : hello [nom], exit\n";

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        if (!input.empty()) {
            processCommand(input);
        }
    }

    return 0;
}