#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

// Command: cp
// Copy files

extern "C" __declspec(dllexport) void execute(const std::vector<std::string>& args) {
    if (args.size() < 3) {
        std::cout << "Usage: cp <source> <destination>\n";
        std::cout << "Copy a file from source to destination.\n";
        return;
    }

    std::string source = args[1];
    std::string destination = args[2];

    // Check if source file exists
    DWORD fileAttrs = GetFileAttributesA(source.c_str());

    if (fileAttrs == INVALID_FILE_ATTRIBUTES) {
        std::cerr << "cp: cannot stat '" << source << "': No such file\n";
        return;
    }

    if (fileAttrs & FILE_ATTRIBUTE_DIRECTORY) {
        std::cerr << "cp: '" << source << "' is a directory (directory copying not supported)\n";
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

    // Copy the file
    if (CopyFileA(source.c_str(), destination.c_str(), FALSE)) {
        std::cout << "File copied from '" << source << "' to '" << destination << "' successfully.\n";
    } else {
        DWORD error = GetLastError();
        std::cerr << "cp: cannot copy '" << source << "' to '" << destination << "': Error " << error << "\n";
    }
}
