#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

// Command: whoami
// Display the current username

extern "C" __declspec(dllexport) void execute(const std::vector<std::string>& args) {
    char username[256];
    DWORD usernameLen = sizeof(username);

    if (GetUserNameA(username, &usernameLen)) {
        std::cout << username << "\n";
    } else {
        std::cerr << "whoami: Unable to retrieve username\n";
    }
}
