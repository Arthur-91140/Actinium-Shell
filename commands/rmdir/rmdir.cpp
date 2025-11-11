#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

// Command: rmdir
// Remove empty directories

extern "C" __declspec(dllexport) void execute(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "Usage: rmdir <directory1> [directory2] ...\n";
        std::cout << "Remove one or more empty directories.\n";
        return;
    }

    for (size_t i = 1; i < args.size(); ++i) {
        std::string dirName = args[i];

        if (RemoveDirectoryA(dirName.c_str())) {
            std::cout << "Directory '" << dirName << "' removed successfully.\n";
        } else {
            DWORD error = GetLastError();
            if (error == ERROR_DIR_NOT_EMPTY) {
                std::cerr << "rmdir: cannot remove '" << dirName << "': Directory not empty\n";
            } else if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) {
                std::cerr << "rmdir: cannot remove '" << dirName << "': No such directory\n";
            } else {
                std::cerr << "rmdir: cannot remove '" << dirName << "': Error " << error << "\n";
            }
        }
    }
}
