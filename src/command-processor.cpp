#include <iostream>
#include <string>
#include <sstream>
#include <locale>
#include <codecvt>
#include <cstdlib>
#include <windows.h>
#include "./include/command-processor.h"

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