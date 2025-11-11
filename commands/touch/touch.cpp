#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>

// Command: touch
// Create empty files or update file timestamps

extern "C" __declspec(dllexport) void execute(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "Usage: touch <file1> [file2] ...\n";
        std::cout << "Create empty files or update file timestamps.\n";
        return;
    }

    for (size_t i = 1; i < args.size(); ++i) {
        std::string filename = args[i];

        // Check if file exists
        DWORD fileAttrs = GetFileAttributesA(filename.c_str());

        if (fileAttrs == INVALID_FILE_ATTRIBUTES) {
            // File doesn't exist, create it
            std::ofstream file(filename);
            if (file.is_open()) {
                file.close();
                std::cout << "File '" << filename << "' created.\n";
            } else {
                std::cerr << "touch: cannot create file '" << filename << "'\n";
            }
        } else {
            // File exists, update timestamp
            HANDLE hFile = CreateFileA(
                filename.c_str(),
                FILE_WRITE_ATTRIBUTES,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL
            );

            if (hFile != INVALID_HANDLE_VALUE) {
                SYSTEMTIME st;
                FILETIME ft;
                GetSystemTime(&st);
                SystemTimeToFileTime(&st, &ft);
                SetFileTime(hFile, NULL, &ft, &ft);
                CloseHandle(hFile);
                std::cout << "File '" << filename << "' timestamp updated.\n";
            } else {
                std::cerr << "touch: cannot update timestamp for '" << filename << "'\n";
            }
        }
    }
}
