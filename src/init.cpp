#include <iostream>
#include <string>
#include <sstream>
#include <locale>
#include <codecvt>
#include <cstdlib>
#include <windows.h>
#include <shlobj.h> // For SHGetFolderPath
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
std::string GetUserFolderPath() {
    char path[MAX_PATH];
    if (SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, path) == S_OK) {
        return std::string(path);
    }
    return "";
}