#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

// Command: echo
// Display a line of text

extern "C" __declspec(dllexport) void execute(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "\n";
        return;
    }

    for (size_t i = 1; i < args.size(); ++i) {
        std::cout << args[i];
        if (i < args.size() - 1) {
            std::cout << " ";
        }
    }
    std::cout << "\n";
}
