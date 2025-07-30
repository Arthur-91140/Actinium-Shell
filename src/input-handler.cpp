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
    
    // Find the part of the suggestion that completes the current input
    const std::string& suggestion = state.suggestions[0];
    if (suggestion.length() > state.currentInput.length() && 
        suggestion.substr(0, state.currentInput.length()) == state.currentInput) {
        
        // Show only the completing part in gray
        std::string completion = suggestion.substr(state.currentInput.length());
        setTextColor(Config::COLOR_SUGGESTION);
        std::cout << completion;
        resetTextColor();
    }
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
    bool shouldExit = false;
    
    while (!shouldExit) {
        ch = _getch();
        
        if (ch == '\r') { // Enter key
            shouldExit = true;
        } else if (ch == 0 || ch == 224) { // Extended key
            ch = _getch();
            switch (ch) {
                case 72: // Up arrow
                    // Show recent commands from history
                    state.suggestions = g_historyManager->getRecentCommands(10);
                    if (!state.suggestions.empty()) {
                        state.showingSuggestions = true;
                        state.suggestionIndex = 0;
                    }
                    break;
                case 80: // Down arrow
                    // Show recent commands from history
                    state.suggestions = g_historyManager->getRecentCommands(10);
                    if (!state.suggestions.empty()) {
                        state.showingSuggestions = true;
                        state.suggestionIndex = 0;
                    }
                    break;
                case 75: // Left arrow
                    if (state.cursorPosition > 0) {
                        state.cursorPosition--;
                    }
                    break;
                case 77: // Right arrow - Accept suggestion
                    if (state.showingSuggestions && !state.suggestions.empty()) {
                        // Accept the suggestion (most probable one)
                        state.currentInput = state.suggestions[0];
                        state.cursorPosition = state.currentInput.length();
                        state.showingSuggestions = false;
                        state.suggestions.clear();
                    } else if (state.cursorPosition < (int)state.currentInput.length()) {
                        state.cursorPosition++;
                    }
                    break;
            }
        } else if (ch == '\t') { // Tab key
            // Tab completion for files and directories
            std::vector<std::string> completions = g_autoComplete->getFileCompletions(state.currentInput);
            if (!completions.empty()) {
                if (completions.size() == 1) {
                    // Auto-complete with the single match
                    state.currentInput = completions[0];
                    state.cursorPosition = state.currentInput.length();
                    state.showingSuggestions = false;
                    state.suggestions.clear();
                } else {
                    // Show the most probable completion
                    state.suggestions = completions;
                    state.showingSuggestions = true;
                    state.suggestionIndex = 0;
                }
            }
        } else if (ch == '\b') { // Backspace
            if (state.cursorPosition > 0) {
                state.currentInput.erase(state.cursorPosition - 1, 1);
                state.cursorPosition--;
                
                // Update suggestions with fuzzy search from history only
                if (!state.currentInput.empty()) {
                    state.suggestions = g_historyManager->fuzzySearch(state.currentInput);
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
        } else if (ch >= 32 && ch <= 126) { // Printable characters
            state.currentInput.insert(state.cursorPosition, 1, (char)ch);
            state.cursorPosition++;
            
            // Update suggestions with fuzzy search from history only
            state.suggestions = g_historyManager->fuzzySearch(state.currentInput);
            state.showingSuggestions = !state.suggestions.empty();
            state.suggestionIndex = 0;
        }
        
        if (!shouldExit) {
            // Clear current line only
            CONSOLE_SCREEN_BUFFER_INFO info;
            GetConsoleScreenBufferInfo(hConsole, &info);
            
            // Clear the current line
            COORD coord = {0, info.dwCursorPosition.Y};
            DWORD written;
            FillConsoleOutputCharacterA(hConsole, ' ', info.dwSize.X, coord, &written);
            SetConsoleCursorPosition(hConsole, coord);
            
            // Redraw everything on the same line
            printPrompt();
            printInput();
            printSuggestions();
            
            // Position cursor correctly in input
            int promptLength = currentDirectory.length() + 5; // "AC " + "> " = 5 chars
            COORD inputCursor = {(SHORT)(promptLength + state.cursorPosition), info.dwCursorPosition.Y};
            SetConsoleCursorPosition(hConsole, inputCursor);
        }
    }
    
    std::cout << std::endl;
    
    // Add to history if not empty
    if (!state.currentInput.empty()) {
        g_historyManager->addCommand(state.currentInput);
    }
    
    return state.currentInput;
}