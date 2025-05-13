// command-processor.h
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <locale>
#include <codecvt>
#include <cstdlib>
#include <windows.h>

// Command module structure
struct CommandModule {
    std::string name;
    std::string sourcePath;
    std::string headerPath;
    std::string binaryPath;
    HMODULE handle;
    void (*execute)(const std::vector<std::string>&);
};

// Global variables
extern std::string currentDirectory;
extern std::map<std::string, CommandModule> loadedCommands;
extern std::string commandsDirectory;

// Command processing functions
void processCommand(const std::string& input);
void listDirectory(const std::string& path);
bool changeDirectory(const std::string& path);
void printCurrentDirectory();
std::string findExecutableInPath(const std::string& command);
void executeExternalCommand(const std::string& command);

// Command module management
void createCommandTemplate(const std::string& name);
bool createCommand(const std::string& name);
bool loadCommandModule(const std::string& name);
bool unloadCommandModule(const std::string& name);
bool compileCommandModule(const std::string& name);
bool editCommandSource(const std::string& name);
void initializeCommandSystem();
void showHelp();