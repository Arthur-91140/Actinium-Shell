#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>

// Command: cat
// Display the contents of one or more files

extern "C" __declspec(dllexport) void execute(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "Usage: cat <file1> [file2] [file3] ...\n";
        std::cout << "Display the contents of one or more files.\n";
        return;
    }

    for (size_t i = 1; i < args.size(); ++i) {
        std::string filename = args[i];
        std::ifstream file(filename);

        if (!file.is_open()) {
            std::cerr << "cat: " << filename << ": No such file or directory\n";
            continue;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::cout << line << "\n";
        }

        file.close();
    }
}
