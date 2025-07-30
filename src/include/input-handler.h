// src/include/input-handler.h
#pragma once
#include <string>
#include <vector>
#include <windows.h>

struct InputState {
    std::string currentInput;
    std::vector<std::string> suggestions;
    int suggestionIndex;
    int cursorPosition;
    bool showingSuggestions;
};

class InputHandler {
private:
    HANDLE hConsole;
    HANDLE hInput;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    InputState state;
    
    void hideCursor();
    void showCursor();
    void clearLine();
    void printPrompt();
    void printInput();
    void printSuggestions();
    void moveCursor(int x, int y);
    void setTextColor(int color);
    void resetTextColor();
    
public:
    InputHandler();
    ~InputHandler();
    std::string readInput();
    void updateSuggestions(const std::vector<std::string>& suggestions);
};