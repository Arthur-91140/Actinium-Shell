#pragma once
#include <vector>
#include <string>

// Command: rm
// Function declaration for DLL export

extern "C" __declspec(dllexport) void execute(const std::vector<std::string>& args);
