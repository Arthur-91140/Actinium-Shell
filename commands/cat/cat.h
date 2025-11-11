#pragma once
#include <vector>
#include <string>

// Command: cat
// Function declaration for DLL export

extern "C" __declspec(dllexport) void execute(const std::vector<std::string>& args);
