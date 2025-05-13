// command-processor.cpp
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <locale>
#include <codecvt>
#include <cstdlib>
#include <windows.h>
#include "./include/command-processor.h"
#include "./include/init.h"

// Global variables
std::string currentDirectory = ".";  // Current directory
std::map<std::string, CommandModule> loadedCommands;
std::string commandsDirectory = GetUserFolderPath() + "\\commands";

// Create a template for new commands
void createCommandTemplate(const std::string& name) {
    std::string srcPath = commandsDirectory + "\\" + name + "\\" + name + ".cpp";
    
    // Create a basic template file
    FILE* file = fopen(srcPath.c_str(), "w");
    if (file) {
        fprintf(file, "#include <iostream>\n");
        fprintf(file, "#include <string>\n");
        fprintf(file, "#include <vector>\n\n");
        fprintf(file, "// Command: %s\n", name.c_str());
        fprintf(file, "// This is an automatically generated template.\n\n");
        fprintf(file, "extern \"C\" __declspec(dllexport) void execute(const std::vector<std::string>& args) {\n");
        fprintf(file, "    std::cout << \"This is the %s command.\" << std::endl;\n", name.c_str());
        fprintf(file, "    std::cout << \"It was called with \" << args.size() - 1 << \" arguments.\" << std::endl;\n");
        fprintf(file, "    \n");
        fprintf(file, "    // Your command implementation goes here\n");
        fprintf(file, "}\n");
        fclose(file);
    }
    
    // Also create a header file
    std::string headerPath = commandsDirectory + "\\" + name + "\\" + name + ".h";
    file = fopen(headerPath.c_str(), "w");
    if (file) {
        fprintf(file, "#pragma once\n");
        fprintf(file, "#include <vector>\n");
        fprintf(file, "#include <string>\n\n");
        fprintf(file, "// Command: %s\n", name.c_str());
        fprintf(file, "// Function declaration for DLL export\n\n");
        fprintf(file, "extern \"C\" __declspec(dllexport) void execute(const std::vector<std::string>& args);\n");
        fclose(file);
    }
}

// Function to create a new command
bool createCommand(const std::string& name) {
    // Check if the command already exists
    if (loadedCommands.find(name) != loadedCommands.end()) {
        std::cout << "Error: Command '" << name << "' already exists.\n";
        return false;
    }
    
    // Create command directory
    std::string moduleDirPath = commandsDirectory + "\\" + name;
    if (!CreateDirectoryA(moduleDirPath.c_str(), NULL)) {
        if (GetLastError() != ERROR_ALREADY_EXISTS) {
            std::cout << "Error: Cannot create directory for command '" << name << "'.\n";
            return false;
        }
    }
    
    // Create template files
    createCommandTemplate(name);
    
    // Set up command structure
    CommandModule cmd;
    cmd.name = name;
    cmd.sourcePath = moduleDirPath + "\\" + name + ".cpp";
    cmd.headerPath = moduleDirPath + "\\" + name + ".h";
    cmd.binaryPath = moduleDirPath + "\\" + name + ".dll";
    cmd.handle = NULL;
    cmd.execute = NULL;
    
    loadedCommands[name] = cmd;
    
    std::cout << "Command '" << name << "' created successfully.\n";
    std::cout << "Would you like to edit it now? (y/n): ";
    
    char response;
    std::cin >> response;
    std::cin.ignore(); // Clear the newline
    
    if (response == 'y' || response == 'Y') {
        return editCommandSource(name);
    }
    
    return true;
}

// Initialize the command system
void initializeCommandSystem() {
    // Create commands directory if it doesn't exist
    CreateDirectoryA(commandsDirectory.c_str(), NULL);
    
    // Try to load built-in commands
    loadCommandModule("ls");
    loadCommandModule("cd");
    loadCommandModule("pwd");
}

// Load a command module
bool loadCommandModule(const std::string& name) {
    // Check if already loaded
    if (loadedCommands.find(name) != loadedCommands.end()) {
        // If handle exists, module is already loaded
        if (loadedCommands[name].handle != NULL) {
            return true;
        }
    }
    
    // Setup paths
    std::string moduleDirPath = commandsDirectory + "\\" + name;
    std::string srcPath = moduleDirPath + "\\" + name + ".cpp";
    std::string headerPath = moduleDirPath + "\\" + name + ".h";
    std::string binaryPath = moduleDirPath + "\\" + name + ".dll";
    
    // Check if directory exists, create if not
    CreateDirectoryA(moduleDirPath.c_str(), NULL);
    
    // Check if source files exist
    DWORD fileAttrs = GetFileAttributesA(srcPath.c_str());
    if (fileAttrs == INVALID_FILE_ATTRIBUTES) {
        // Source doesn't exist yet, create template
        createCommandTemplate(name);
    }
    
    // Check if binary exists, compile if not
    fileAttrs = GetFileAttributesA(binaryPath.c_str());
    if (fileAttrs == INVALID_FILE_ATTRIBUTES) {
        // Binary doesn't exist, try to compile
        if (!compileCommandModule(name)) {
            std::cout << "Error: Failed to compile command module '" << name << "'.\n";
            return false;
        }
    }
    
    // Try to load the binary
    HMODULE handle = LoadLibraryA(binaryPath.c_str());
    if (handle == NULL) {
        std::cout << "Error: Failed to load command module '" << name << "'.\n";
        return false;
    }
    
    // Get the execute function
    void (*executeFunc)(const std::vector<std::string>&) = 
        (void (*)(const std::vector<std::string>&))GetProcAddress(handle, "execute");
    
    if (executeFunc == NULL) {
        std::cout << "Error: Command module '" << name << "' does not export execute function.\n";
        FreeLibrary(handle);
        return false;
    }
    
    // Store the command info
    CommandModule cmd;
    cmd.name = name;
    cmd.sourcePath = srcPath;
    cmd.headerPath = headerPath;
    cmd.binaryPath = binaryPath;
    cmd.handle = handle;
    cmd.execute = executeFunc;
    
    loadedCommands[name] = cmd;
    return true;
}

// Unload a command module
bool unloadCommandModule(const std::string& name) {
    if (loadedCommands.find(name) == loadedCommands.end()) {
        return false;
    }
    
    if (loadedCommands[name].handle != NULL) {
        FreeLibrary(loadedCommands[name].handle);
        loadedCommands[name].handle = NULL;
        loadedCommands[name].execute = NULL;
    }
    
    return true;
}

// Compile a command module
bool compileCommandModule(const std::string& name) {
    if (loadedCommands.find(name) == loadedCommands.end()) {
        // Setup paths for new command
        std::string moduleDirPath = commandsDirectory + "\\" + name;
        std::string srcPath = moduleDirPath + "\\" + name + ".cpp";
        std::string headerPath = moduleDirPath + "\\" + name + ".h";
        std::string binaryPath = moduleDirPath + "\\" + name + ".dll";
        
        CommandModule cmd;
        cmd.name = name;
        cmd.sourcePath = srcPath;
        cmd.headerPath = headerPath;
        cmd.binaryPath = binaryPath;
        cmd.handle = NULL;
        cmd.execute = NULL;
        
        loadedCommands[name] = cmd;
    }
    
    // Unload the module if it's loaded
    unloadCommandModule(name);
    
    // Build the compilation command
    std::string compileCmd = "g++ -shared -o \"" + loadedCommands[name].binaryPath + 
                           "\" \"" + loadedCommands[name].sourcePath + "\" -I./src";
    
    // Execute the compilation command
    std::cout << "Compiling command module '" << name << "'...\n";
    int result = system(compileCmd.c_str());
    
    if (result != 0) {
        std::cout << "Error: Compilation failed with exit code " << result << ".\n";
        return false;
    }
    
    std::cout << "Compilation successful.\n";
    return true;
}

// Edit a command source
bool editCommandSource(const std::string& name) {
    // Check if the command exists
    if (loadedCommands.find(name) == loadedCommands.end()) {
        // Try to load it
        if (!loadCommandModule(name)) {
            std::cout << "Error: Command '" << name << "' does not exist. Create it? (y/n): ";
            char response;
            std::cin >> response;
            std::cin.ignore(); // Clear the newline
            
            if (response == 'y' || response == 'Y') {
                // Create new command
                std::string moduleDirPath = commandsDirectory + "\\" + name;
                CreateDirectoryA(moduleDirPath.c_str(), NULL);
                createCommandTemplate(name);
                
                CommandModule cmd;
                cmd.name = name;
                cmd.sourcePath = moduleDirPath + "\\" + name + ".cpp";
                cmd.headerPath = moduleDirPath + "\\" + name + ".h";
                cmd.binaryPath = moduleDirPath + "\\" + name + ".dll";
                cmd.handle = NULL;
                cmd.execute = NULL;
                
                loadedCommands[name] = cmd;
            } else {
                return false;
            }
        }
    }
    
    // Open the source file in the default editor (notepad for simplicity)
    std::string editorCmd = "notepad.exe \"" + loadedCommands[name].sourcePath + "\"";
    system(editorCmd.c_str());
    
    // Ask if user wants to compile
    std::cout << "Recompile the command? (y/n): ";
    char response;
    std::cin >> response;
    std::cin.ignore(); // Clear the newline
    
    if (response == 'y' || response == 'Y') {
        return compileCommandModule(name);
    }
    
    return true;
}

// Function to display help
void showHelp() {
    std::cout << "Available Commands:\n";
    std::cout << "=================\n";
    
    // Built-in commands
    std::cout << "Built-in commands:\n";
    std::cout << "  hello [name]    - Display a welcome message\n";
    std::cout << "  exit            - Quit the shell\n";
    std::cout << "  pwd             - Show current directory\n";
    std::cout << "  ls [path]       - List directory contents\n";
    std::cout << "  cd [path]       - Change current directory\n";
    std::cout << "  help            - Display this help\n";
    
    // Command management
    std::cout << "\nCommand Management:\n";
    std::cout << "  create [name]   - Create a new command\n";
    std::cout << "  edit [name]     - Edit an existing command\n";
    std::cout << "  compile [name]  - Compile a command module\n";
    std::cout << "  reload [name]   - Reload a command module\n";
    
    // Dynamic commands
    if (!loadedCommands.empty()) {
        std::cout << "\nDynamic Commands:\n";
        for (const auto& cmd : loadedCommands) {
            if (cmd.second.handle != NULL) {
                std::cout << "  " << cmd.first << "\n";
            }
        }
    }
    
    std::cout << "\nTip: You can modify any command by using 'edit [command]'.\n";
}

// Original command functions
void listDirectory(const std::string& path) {
    std::string searchPath = path + "\\*";
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        std::cout << "Error: Unable to list directory.\n";
        return;
    }

    do {
        std::string fileName = findData.cFileName;
        if (fileName != "." && fileName != "..") {
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                std::cout << "[DIR] " << fileName << "\n";
            } else {
                std::cout << "[FILE] " << fileName << "\n";
            }
        }
    } while (FindNextFileA(hFind, &findData));

    FindClose(hFind);
}

bool changeDirectory(const std::string& path) {
    // If absolute path
    if (path.size() > 1 && path[1] == ':') {
        if (SetCurrentDirectoryA(path.c_str())) {
            char buffer[MAX_PATH];
            GetCurrentDirectoryA(MAX_PATH, buffer);
            currentDirectory = buffer;
            return true;
        }
        return false;
    }
    
    // If relative path
    std::string newPath = currentDirectory;
    if (path == "..") {
        // Go up one level
        size_t pos = newPath.find_last_of('\\');
        if (pos != std::string::npos && pos > 0) {
            newPath = newPath.substr(0, pos);
            // If only drive letter, add "\"
            if (newPath.size() == 2 && newPath[1] == ':') {
                newPath += "\\";
            }
        }
    } else if (path != ".") {
        // Go down to a subdirectory
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

void printCurrentDirectory() {
    std::cout << currentDirectory << "\n";
}


// Améliorations pour la fonction findExecutableInPath
std::string findExecutableInPath(const std::string& command) {
    // Si la commande contient déjà un chemin complet
    if (command.find('\\') != std::string::npos || command.find('/') != std::string::npos) {
        DWORD fileAttrs = GetFileAttributesA(command.c_str());
        if (fileAttrs != INVALID_FILE_ATTRIBUTES && !(fileAttrs & FILE_ATTRIBUTE_DIRECTORY)) {
            return command;
        }
    }
    
    // Récupérer la variable d'environnement PATH
    char pathBuffer[32767]; // Taille maximale pour les variables d'environnement
    DWORD pathSize = GetEnvironmentVariableA("PATH", pathBuffer, sizeof(pathBuffer));
    
    if (pathSize == 0 || pathSize >= sizeof(pathBuffer)) {
        return ""; // Échec de la récupération du PATH ou tampon trop petit
    }
    
    std::string path(pathBuffer);
    std::istringstream pathStream(path);
    std::string pathDir;
    
    // Extensions communes des exécutables sous Windows
    std::vector<std::string> extensions = {".exe", ".com", ".bat", ".cmd"};
    
    // Si la commande n'a pas d'extension, essayer avec les extensions communes
    if (command.find('.') == std::string::npos) {
        for (const auto& ext : extensions) {
            std::istringstream pathStreamCopy(path);
            while (std::getline(pathStreamCopy, pathDir, ';')) {
                if (pathDir.empty()) continue;
                
                // S'assurer que le chemin se termine par un backslash
                if (pathDir.back() != '\\') {
                    pathDir += '\\';
                }
                
                std::string fullPath = pathDir + command + ext;
                
                // Vérifier si le fichier existe et n'est pas un répertoire
                DWORD fileAttrs = GetFileAttributesA(fullPath.c_str());
                if (fileAttrs != INVALID_FILE_ATTRIBUTES && !(fileAttrs & FILE_ATTRIBUTE_DIRECTORY)) {
                    return fullPath;
                }
            }
        }
    } else {
        // La commande a déjà une extension, la chercher directement
        while (std::getline(pathStream, pathDir, ';')) {
            if (pathDir.empty()) continue;
            
            // S'assurer que le chemin se termine par un backslash
            if (pathDir.back() != '\\') {
                pathDir += '\\';
            }
            
            std::string fullPath = pathDir + command;
            
            // Vérifier si le fichier existe et n'est pas un répertoire
            DWORD fileAttrs = GetFileAttributesA(fullPath.c_str());
            if (fileAttrs != INVALID_FILE_ATTRIBUTES && !(fileAttrs & FILE_ATTRIBUTE_DIRECTORY)) {
                return fullPath;
            }
        }
    }
    
    return ""; // Non trouvé dans le PATH
}

// Amélioration de la fonction executeExternalCommand
void executeExternalCommand(const std::string& command) {
    // Analyser la commande pour obtenir le nom de l'exécutable et les arguments
    std::istringstream iss(command);
    std::string execName;
    iss >> execName;
    
    // Trouver l'exécutable dans le PATH
    std::string execPath = findExecutableInPath(execName);
    
    if (execPath.empty()) {
        std::cout << "Erreur: Commande '" << execName << "' introuvable.\n";
        return;
    }
    
    // Créer des pipes pour rediriger la sortie
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    HANDLE hReadPipe, hWritePipe;
    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
        std::cout << "Erreur: Impossible de créer les pipes pour la redirection.\n";
        return;
    }
    
    SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0);

    // Préparer les structures pour CreateProcess
    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(STARTUPINFOA));
    si.cb = sizeof(STARTUPINFOA);
    si.hStdError = hWritePipe;
    si.hStdOutput = hWritePipe;
    si.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

    // Créer une copie modifiable de la commande
    char* commandLine = _strdup(command.c_str());

    // Exécuter la commande
    BOOL success = CreateProcessA(
        NULL,               // Pas de module spécifique
        commandLine,        // Ligne de commande
        NULL,               // Sécurité de processus par défaut
        NULL,               // Sécurité de thread par défaut
        TRUE,               // Hériter les handles
        0,                  // Pas de flags de création
        NULL,               // Utiliser l'environnement du parent
        NULL,               // Utiliser le répertoire courant
        &si,                // Info de démarrage
        &pi                 // Info de processus
    );

    // Libérer la mémoire allouée
    free(commandLine);

    if (!success) {
        DWORD error = GetLastError();
        std::cout << "Erreur: Impossible d'exécuter la commande. Code d'erreur: " << error << "\n";
        CloseHandle(hWritePipe);
        CloseHandle(hReadPipe);
        return;
    }

    // Fermer le handle d'écriture pour que ReadFile puisse se terminer
    CloseHandle(hWritePipe);

    // Lire la sortie du processus
    const int BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];
    DWORD bytesRead;
    std::string output;
    
    while (ReadFile(hReadPipe, buffer, BUFFER_SIZE - 1, &bytesRead, NULL) && bytesRead != 0) {
        buffer[bytesRead] = '\0';  // Terminer la chaîne par un null
        std::cout << buffer;
        output += buffer;
    }

    // Attendre que le processus se termine
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Récupérer le code de sortie
    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);

    // Nettoyer
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hReadPipe);
}

// Amélioration de la fonction processCommand pour mieux gérer les commandes externes
void processCommand(const std::string& input) {
    std::istringstream iss(input);
    std::string cmd;
    iss >> cmd;
    
    if (cmd.empty()) return;
    
    // Créer un vecteur d'arguments
    std::vector<std::string> args;
    args.push_back(cmd); // Le premier argument est la commande elle-même
    
    std::string arg;
    while (iss >> arg) {
        args.push_back(arg);
    }
    
    // Vérifier les commandes intégrées
    if (cmd == "hello") {
        if (args.size() > 1) {
            std::cout << "Hello, " << args[1] << "! \n";
        } else {
            std::cout << "Hello, world! \n";
        }
    } else if (cmd == "exit") {
        std::cout << "Fermeture du terminal. \n";
        exit(0);
    } else if (cmd == "pwd") {
        printCurrentDirectory();
    } else if (cmd == "cd") {
        if (args.size() > 1) {
            if (changeDirectory(args[1])) {
                std::cout << currentDirectory << "\n";
            } else {
                std::cout << "Erreur: Impossible de changer de répertoire.\n";
            }
        } else {
            // Par défaut, aller au répertoire utilisateur si aucun chemin n'est spécifié
            std::string userHome = GetUserFolderPath();
            if (!userHome.empty() && changeDirectory(userHome)) {
                std::cout << currentDirectory << "\n";
            } else {
                std::cout << "Erreur: Impossible de changer de répertoire.\n";
            }
        }
    } else if (cmd == "ls") {
        if (args.size() > 1) {
            listDirectory(args[1]);
        } else {
            listDirectory(currentDirectory);
        }
    } else if (cmd == "help") {
        showHelp();
    } else if (cmd == "version") {
        std::cout << "AC Command Processor v1.0 INDEV\n";
        std::cout << "  Built on " << __DATE__ << " at " << __TIME__ << "\n";
        std::cout << "  Author: Arthur Pruvost Rivière\n";
        std::cout << "\n";
        std::cout << "Squirel Packet Manager [not implemented] INDEV\n";
        std::cout << "  Built on " << __DATE__ << " at " << __TIME__ << "\n";
        std::cout << "  Author: Arthur Pruvost Rivière\n";
        std::cout << "\n";
        std::cout << "Packet Version :\n";
        std::cout << "  No Packet Downloaded\n";
    } else if (cmd == "create") {
        if (args.size() > 1) {
            createCommand(args[1]);
        } else {
            std::cout << "Erreur: Veuillez spécifier un nom de commande à créer.\n";
        }
    } else if (cmd == "edit") {
        if (args.size() > 1) {
            editCommandSource(args[1]);
        } else {
            std::cout << "Erreur: Veuillez spécifier une commande à éditer.\n";
        }
    } else if (cmd == "compile") {
        if (args.size() > 1) {
            compileCommandModule(args[1]);
        } else {
            std::cout << "Erreur: Veuillez spécifier une commande à compiler.\n";
        }
    } else if (cmd == "reload") {
        if (args.size() > 1) {
            unloadCommandModule(args[1]);
            loadCommandModule(args[1]);
        } else {
            std::cout << "Erreur: Veuillez spécifier une commande à recharger.\n";
        }
    } else {
        // *** MODIFICATION IMPORTANTE : D'abord vérifier dans le PATH ***
        // Vérifier si c'est une commande dans le PATH
        std::string execPath = findExecutableInPath(cmd);
        if (!execPath.empty()) {
            // C'est une commande externe, l'exécuter
            executeExternalCommand(input);
        }
        // Sinon, vérifier si c'est une commande chargée
        else if (loadedCommands.find(cmd) != loadedCommands.end() && loadedCommands[cmd].execute != NULL) {
            // Exécuter la commande
            loadedCommands[cmd].execute(args);
        } 
        // Enfin, essayer de la charger comme module
        else if (loadCommandModule(cmd)) {
            loadedCommands[cmd].execute(args);
        } 
        else {
            std::cout << "Erreur: Commande '" << cmd << "' introuvable.\n";
        }
    }
}