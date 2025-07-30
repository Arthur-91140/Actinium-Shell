// src/input-handler.cpp
#include "./include/input-handler.h"
#include "./include/config.h"
#include "./include/command-processor.h"
#include "./include/auto-complete.h"
#include "./include/history-manager.h"
#include <iostream>
#include <conio.h>

extern HistoryManager* g_historyManager;
extern AutoComplete* g_autoComplete;

InputHandler::InputHandler() {
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    hInput = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    
    state.currentInput = "";
    state.suggestionIndex = -1;
    state.cursorPosition = 0;
    state.showingSuggestions = false;
}

InputHandler::~InputHandler() {
    resetTextColor();
    showCursor();
}

void InputHandler::hideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void InputHandler::showCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = true;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void InputHandler::clearLine() {
    COORD coord = {0, csbi.dwCursorPosition.Y};
    DWORD written;
    FillConsoleOutputCharacterA(hConsole, ' ', csbi.dwSize.X, coord, &written);
    SetConsoleCursorPosition(hConsole, coord);
}

void InputHandler::printPrompt() {
    setTextColor(Config::COLOR_PROMPT);
    std::cout << "AC " << currentDirectory << "> ";
    resetTextColor();
}

void InputHandler::printInput() {
    setTextColor(Config::COLOR_INPUT);
    std::cout << state.currentInput;
    resetTextColor();
}

void InputHandler::printSuggestions() {
    if (!state.showingSuggestions || state.suggestions.empty()) {
        return;
    }
    
    // Save current cursor position
    CONSOLE_SCREEN_BUFFER_INFO currentPos;
    GetConsoleScreenBufferInfo(hConsole, &currentPos);
    
    // Move to next line
    COORD newPos = {0, currentPos.dwCursorPosition.Y + 1};
    SetConsoleCursorPosition(hConsole, newPos);
    
    setTextColor(Config::COLOR_SUGGESTION);
    
    int count = 0;
    for (size_t i = 0; i < state.suggestions.size() && count < Config::MAX_SUGGESTIONS; ++i) {
        if (i == state.suggestionIndex) {
            // Highlight selected suggestion
            SetConsoleTextAttribute(hConsole, BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        }
        std::cout << "  " << state.suggestions[i];
        if (i == state.suggestionIndex) {
            resetTextColor();
            setTextColor(Config::COLOR_SUGGESTION);
        }
        
        if (i < state.suggestions.size() - 1 && count < Config::MAX_SUGGESTIONS - 1) {
            std::cout << "\n";
            count++;
        }
    }
    
    resetTextColor();
    
    // Return to input line
    SetConsoleCursorPosition(hConsole, currentPos);
}

void InputHandler::moveCursor(int x, int y) {
    COORD coord = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(hConsole, coord);
}

void InputHandler::setTextColor(int color) {
    SetConsoleTextAttribute(hConsole, color);
}

void InputHandler::resetTextColor() {
    SetConsoleTextAttribute(hConsole, Config::COLOR_NORMAL);
}

std::string InputHandler::readInput() {
    state.currentInput.clear();
    state.cursorPosition = 0;
    state.suggestionIndex = -1;
    state.showingSuggestions = false;
    state.suggestions.clear();
    
    printPrompt();
    
    int ch;
    while ((ch = _getch()) != '\r') { // Enter key
        if (ch == 0 || ch == 224) { // Extended key
            ch = _getch();
            switch (ch) {
                case 72: // Up arrow
                    if (state.showingSuggestions && state.suggestionIndex > 0) {
                        state.suggestionIndex--;
                    } else if (!state.showingSuggestions) {
                        // Show recent commands
                        state.suggestions = g_historyManager->getRecentCommands();
                        state.showingSuggestions = true;
                        state.suggestionIndex = 0;
                    }
                    break;
                case 80: // Down arrow
                    if (state.showingSuggestions && state.suggestionIndex < (int)state.suggestions.size() - 1) {
                        state.suggestionIndex++;
                    }
                    break;
                case 75: // Left arrow
                    if (state.cursorPosition > 0) {
                        state.cursorPosition--;
                    }
                    break;
                case 77: // Right arrow
                    if (state.cursorPosition < (int)state.currentInput.length()) {
                        state.cursorPosition++;
                    }
                    break;
            }
        } else if (ch == '\t') { // Tab key
            if (!state.currentInput.empty()) {
                std::vector<std::string> completions = g_autoComplete->getCompletions(state.currentInput);
                if (!completions.empty()) {
                    state.suggestions = completions;
                    state.showingSuggestions = true;
                    state.suggestionIndex = 0;
                }
            }
        } else if (ch == '\b') { // Backspace
            if (state.cursorPosition > 0) {
                state.currentInput.erase(state.cursorPosition - 1, 1);
                state.cursorPosition--;
                
                // Update suggestions with fuzzy search
                if (!state.currentInput.empty()) {
                    std::vector<std::string> historySuggestions = g_historyManager->fuzzySearch(state.currentInput);
                    std::vector<std::string> commandSuggestions = g_autoComplete->getCommandSuggestions(state.currentInput);
                    
                    state.suggestions.clear();
                    state.suggestions.insert(state.suggestions.end(), historySuggestions.begin(), historySuggestions.end());
                    state.suggestions.insert(state.suggestions.end(), commandSuggestions.begin(), commandSuggestions.end());
                    
                    state.showingSuggestions = !state.suggestions.empty();
                    state.suggestionIndex = 0;
                } else {
                    state.showingSuggestions = false;
                    state.suggestions.clear();
                }
            }
        } else if (ch == 27) { // Escape key
            state.showingSuggestions = false;
            state.suggestions.clear();
            state.suggestionIndex = -1;
        } else if (ch == '\r' || ch == '\n') { // Enter with suggestion
            if (state.showingSuggestions && state.suggestionIndex >= 0 && state.suggestionIndex < (int)state.suggestions.size()) {
                state.currentInput = state.suggestions[state.suggestionIndex];
                state.cursorPosition = state.currentInput.length();
                state.showingSuggestions = false;
                state.suggestions.clear();
                break;
            }
        } else if (ch >= 32 && ch <= 126) { // Printable characters
            state.currentInput.insert(state.cursorPosition, 1, (char)ch);
            state.cursorPosition++;
            
            // Update suggestions with fuzzy search
            std::vector<std::string> historySuggestions = g_historyManager->fuzzySearch(state.currentInput);
            std::vector<std::string> commandSuggestions = g_autoComplete->getCommandSuggestions(state.currentInput);
            
            state.suggestions.clear();
            state.suggestions.insert(state.suggestions.end(), historySuggestions.begin(), historySuggestions.end());
            state.suggestions.insert(state.suggestions.end(), commandSuggestions.begin(), commandSuggestions.end());
            
            state.showingSuggestions = !state.suggestions.empty();
            state.suggestionIndex = 0;
        }
        
        // Clear screen area and redraw
        CONSOLE_SCREEN_BUFFER_INFO info;
        GetConsoleScreenBufferInfo(hConsole, &info);
        
        // Clear multiple lines if needed
        for (int i = 0; i < 10; ++i) {
            COORD coord = {0, info.dwCursorPosition.Y + i};
            DWORD written;
            FillConsoleOutputCharacterA(hConsole, ' ', info.dwSize.X, coord, &written);
        }
        
        // Reset cursor to beginning of line
        COORD coord = {0, info.dwCursorPosition.Y};
        SetConsoleCursorPosition(hConsole, coord);
        
        // Redraw everything
        printPrompt();
        printInput();
        printSuggestions();
        
        // Position cursor correctly in input
        int promptLength = currentDirectory.length() + 5; // "AC " + "> " = 5 chars
        COORD inputCursor = {(SHORT)(promptLength + state.cursorPosition), info.dwCursorPosition.Y};
        SetConsoleCursorPosition(hConsole, inputCursor);
    }
    
    // Clear suggestions area
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(hConsole, &info);
    for (int i = 1; i <= Config::MAX_SUGGESTIONS; ++i) {
        COORD coord = {0, info.dwCursorPosition.Y + i};
        DWORD written;
        FillConsoleOutputCharacterA(hConsole, ' ', info.dwSize.X, coord, &written);
    }
    
    std::cout << std::endl;
    
    // Add to history if not empty
    if (!state.currentInput.empty()) {
        g_historyManager->addCommand(state.currentInput);
    }
    
    return state.currentInput;
}