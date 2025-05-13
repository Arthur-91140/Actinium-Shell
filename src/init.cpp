#include <iostream>
#include <string>
#include <sstream>
#include <locale>
#include <codecvt>
#include <cstdlib>
#include <windows.h>
#include "./include/command-processor.h"
#include "./include/init.h"

void banner() {
    std::cout << "  ▄▄▄       ▄████▄  ▄▄▄█████▓ ██▓ ███▄    █  ██▓ █    ██  ███▄ ▄███▓    " << std::endl;
    std::cout << "  ▒████▄    ▒██▀ ▀█  ▓  ██▒ ▓▒▓██▒ ██ ▀█   █ ▓██▒ ██  ▓██▒▓██▒▀█▀ ██▒   " << std::endl;
    std::cout << "  ▒██  ▀█▄  ▒▓█    ▄ ▒ ▓██░ ▒░▒██▒▓██  ▀█ ██▒▒██▒▓██  ▒██░▓██    ▓██░   " << std::endl;
    std::cout << "  ░██▄▄▄▄██ ▒▓▓▄ ▄██▒░ ▓██▓ ░ ░██░▓██▒  ▐▌██▒░██░▓▓█  ░██░▒██    ▒██    " << std::endl; 
    std::cout << "   ▓█   ▓██▒▒ ▓███▀ ░  ▒██▒ ░ ░██░▒██░   ▓██░░██░▒▒█████▓ ▒██▒   ░██▒   " << std::endl;
    std::cout << "   ▒▒   ▓▒█░░ ░▒ ▒  ░  ▒ ░░   ░▓  ░ ▒░   ▒ ▒ ░▓  ░▒▓▒ ▒ ▒ ░ ▒░   ░  ░   " << std::endl;
    std::cout << "    ▒   ▒▒ ░  ░  ▒       ░     ▒ ░░ ░░   ░ ▒░ ▒ ░░░▒░ ░ ░ ░  ░      ░   " << std::endl;
    std::cout << "    ░   ▒   ░          ░       ▒ ░   ░   ░ ░  ▒ ░ ░░░ ░ ░ ░      ░      " << std::endl;
    std::cout << "        ░  ░░ ░                ░           ░  ░     ░            ░      " << std::endl;
    std::cout << "            ░                                                           " << std::endl;
}

// Fonction pour obtenir le dossier utilisateur
string GetUserFolderPath() {
    char path[MAX_PATH];
    if (SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, path) == S_OK) {
        return string(path);
    }
    return "";
}