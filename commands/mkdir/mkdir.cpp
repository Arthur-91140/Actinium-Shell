#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

// Command: mkdir
// Create directories

extern "C" __declspec(dllexport) void execute(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "Usage: mkdir <directory1> [directory2] ...\n";
        std::cout << "Create one or more directories.\n";
        return;
    }

    for (size_t i = 1; i < args.size(); ++i) {
        std::string dirName = args[i];

        if (CreateDirectoryA(dirName.c_str(), NULL)) {
            std::cout << "Directory '" << dirName << "' created successfully.\n";
        } else {
            DWORD error = GetLastError();
            if (error == ERROR_ALREADY_EXISTS) {
                std::cerr << "mkdir: cannot create directory '" << dirName << "': Directory already exists\n";
            } else {
                std::cerr << "mkdir: cannot create directory '" << dirName << "': Error " << error << "\n";
            }
        }
    }
}
