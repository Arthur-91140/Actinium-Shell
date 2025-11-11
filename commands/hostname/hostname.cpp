#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

// Command: hostname
// Display the computer name

extern "C" __declspec(dllexport) void execute(const std::vector<std::string>& args) {
    char computerName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD computerNameLen = sizeof(computerName);

    if (GetComputerNameA(computerName, &computerNameLen)) {
        std::cout << computerName << "\n";
    } else {
        std::cerr << "hostname: Unable to retrieve computer name\n";
    }
}
