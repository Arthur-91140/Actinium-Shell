#include <iostream>
#include <string>
#include <sstream>
#include <locale>
#include <codecvt>
#include <cstdlib>
#include <windows.h>

extern std::string currentDirectory;  // Variable globale pour le répertoire courant

void processCommand(const std::string& input);
void listDirectory(const std::string& path);
bool changeDirectory(const std::string& path);
void printCurrentDirectory();