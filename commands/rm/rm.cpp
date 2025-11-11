#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

// Command: rm
// Remove files

extern "C" __declspec(dllexport) void execute(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "Usage: rm <file1> [file2] ...\n";
        std::cout << "Remove one or more files.\n";
        std::cout << "Warning: This operation is irreversible!\n";
        return;
    }

    for (size_t i = 1; i < args.size(); ++i) {
        std::string filename = args[i];

        // Check if it's a file (not a directory)
        DWORD fileAttrs = GetFileAttributesA(filename.c_str());

        if (fileAttrs == INVALID_FILE_ATTRIBUTES) {
            std::cerr << "rm: cannot remove '" << filename << "': No such file\n";
            continue;
        }

        if (fileAttrs & FILE_ATTRIBUTE_DIRECTORY) {
            std::cerr << "rm: cannot remove '" << filename << "': Is a directory (use rmdir instead)\n";
            continue;
        }

        if (DeleteFileA(filename.c_str())) {
            std::cout << "File '" << filename << "' removed successfully.\n";
        } else {
            DWORD error = GetLastError();
            std::cerr << "rm: cannot remove '" << filename << "': Error " << error << "\n";
        }
    }
}
