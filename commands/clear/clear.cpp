#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

// Command: clear
// Clear the terminal screen

extern "C" __declspec(dllexport) void execute(const std::vector<std::string>& args) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD count;
    DWORD cellCount;
    COORD homeCoords = { 0, 0 };

    // Get the number of cells in the current buffer
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        return;
    }
    cellCount = csbi.dwSize.X * csbi.dwSize.Y;

    // Fill the entire buffer with spaces
    if (!FillConsoleOutputCharacter(
        hConsole,
        (TCHAR)' ',
        cellCount,
        homeCoords,
        &count
    )) {
        return;
    }

    // Fill the entire buffer with the current colors and attributes
    if (!FillConsoleOutputAttribute(
        hConsole,
        csbi.wAttributes,
        cellCount,
        homeCoords,
        &count
    )) {
        return;
    }

    // Move the cursor home
    SetConsoleCursorPosition(hConsole, homeCoords);
}
