#include <iostream>
#include <string>
#include <sstream>
#include <locale>
#include <codecvt>
#include <cstdlib>
#include <windows.h>
#include "./include/command-processor.h"
#include "./include/init.h"

// Ajoutez ceci en haut du fichier, après les includes
std::string currentDirectory = ".";  // Répertoire courant, par défaut le répertoire d'exécution

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
    } else if (cmd == "pwd") {
        printCurrentDirectory();
    } else if (cmd == "ls") {
        std::string path;
        if (iss >> path) {
            listDirectory(path);
        } else {
            listDirectory(currentDirectory);
        }
    } else if (cmd == "cd") {
        std::string path;
        if (iss >> path) {
            if (changeDirectory(path)) {
                std::cout << "Nouveau répertoire: " << currentDirectory << "\n";
            } else {
                std::cout << "Erreur: Impossible de changer de répertoire.\n";
            }
        } else {
            std::cout << "Erreur: Veuillez spécifier un chemin.\n";
        }
    } else {
        std::cout << "Commande inconnue. Commandes disponibles : hello, ls, cd, pwd, exit\n";
    }
}

void listDirectory(const std::string& path) {
    std::string searchPath = path + "\\*";
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        std::cout << "Erreur: Impossible de lister le répertoire.\n";
        return;
    }

    do {
        std::string fileName = findData.cFileName;
        if (fileName != "." && fileName != "..") {
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                std::cout << "[DIR] " << fileName << "\n";
            } else {
                std::cout << "[FIC] " << fileName << "\n";
            }
        }
    } while (FindNextFileA(hFind, &findData));

    FindClose(hFind);
}

bool changeDirectory(const std::string& path) {
    // Si chemin absolu
    if (path.size() > 1 && path[1] == ':') {
        if (SetCurrentDirectoryA(path.c_str())) {
            char buffer[MAX_PATH];
            GetCurrentDirectoryA(MAX_PATH, buffer);
            currentDirectory = buffer;
            return true;
        }
        return false;
    }
    
    // Si chemin relatif
    std::string newPath = currentDirectory;
    if (path == "..") {
        // Remonter d'un niveau
        size_t pos = newPath.find_last_of('\\');
        if (pos != std::string::npos && pos > 0) {
            newPath = newPath.substr(0, pos);
            // Si on a juste la lettre du lecteur, ajouter "\"
            if (newPath.size() == 2 && newPath[1] == ':') {
                newPath += "\\";
            }
        }
    } else if (path != ".") {
        // Descendre dans un sous-répertoire
        newPath += "\\" + path;
    }

    if (SetCurrentDirectoryA(newPath.c_str())) {
        char buffer[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, buffer);
        currentDirectory = buffer;
        return true;
    }
    return false;
}

// Fonction pour afficher le répertoire courant
void printCurrentDirectory() {
    std::cout << "Répertoire courant: " << currentDirectory << "\n";
}