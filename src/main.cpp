#include <iostream>
#include <string>
#include <sstream>
#include <locale>
#include <codecvt>
#include <cstdlib>
#ifdef _WIN32
#include <windows.h>
#endif

void processCommand(const std::string& input) {
    std::istringstream iss(input);
    std::string cmd;
    iss >> cmd;

    if (cmd == "hello") {
        std::string name;
        if (iss >> name) {
            std::cout << "Bonjour, " << name << " ! 👋\n";
        } else {
            std::cout << "Bonjour le monde ! 🌍\n";
        }
    } else if (cmd == "exit") {
        std::cout << "Fermeture du terminal. 👋\n";
        exit(0);
    } else {
        std::cout << "Commande inconnue. Commandes disponibles : hello, exit\n";
    }
}

int main() {
    #ifdef _WIN32
    SetConsoleOutputCP(65001);
    #else
    std::locale::global(std::locale("fr_FR.UTF-8"));
    std::cout.imbue(std::locale());
    #endif

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