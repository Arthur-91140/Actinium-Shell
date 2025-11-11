#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

// Command: mv
// Move or rename files

extern "C" __declspec(dllexport) void execute(const std::vector<std::string>& args) {
    if (args.size() < 3) {
        std::cout << "Usage: mv <source> <destination>\n";
        std::cout << "Move or rename a file from source to destination.\n";
        return;
    }

    std::string source = args[1];
    std::string destination = args[2];

    // Check if source exists
    DWORD fileAttrs = GetFileAttributesA(source.c_str());

    if (fileAttrs == INVALID_FILE_ATTRIBUTES) {
        std::cerr << "mv: cannot stat '" << source << "': No such file or directory\n";
        return;
    }

    // Check if destination is a directory
    DWORD destAttrs = GetFileAttributesA(destination.c_str());
    if (destAttrs != INVALID_FILE_ATTRIBUTES && (destAttrs & FILE_ATTRIBUTE_DIRECTORY)) {
        // Extract filename from source and append to destination directory
        size_t pos = source.find_last_of("\\/");
        std::string filename = (pos == std::string::npos) ? source : source.substr(pos + 1);
        destination = destination + "\\" + filename;
    }

    // Move the file
    if (MoveFileA(source.c_str(), destination.c_str())) {
        std::cout << "Moved '" << source << "' to '" << destination << "' successfully.\n";
    } else {
        DWORD error = GetLastError();
        if (error == ERROR_ALREADY_EXISTS) {
            std::cerr << "mv: cannot move '" << source << "' to '" << destination << "': File already exists\n";
        } else {
            std::cerr << "mv: cannot move '" << source << "' to '" << destination << "': Error " << error << "\n";
        }
    }
}
