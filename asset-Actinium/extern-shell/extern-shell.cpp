#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <windows.h>

extern "C" __declspec(dllexport) void execute(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Usage: extern-shell [--pwsh5 | --pwsh7 | --cmd] <command>" << std::endl;
        return;
    }
    std::string shellType;
    size_t index = 0;
    // Si args[0] est "extern-shell", on l'ignore
    if (args[0] == "extern-shell") {
        if (args.size() < 3) {
            std::cerr << "Usage: extern-shell [--pwsh5 | --pwsh7 | --cmd] <command>" << std::endl;
            return;
        }
        shellType = args[1];
        index = 2;
    } else {
        if (args.size() < 2) {
            std::cerr << "Usage: extern-shell [--pwsh5 | --pwsh7 | --cmd] <command>" << std::endl;
            return;
        }
        shellType = args[0];
        index = 1;
    }
    // Recomposer la commande
    std::string command;
    for (size_t i = index; i < args.size(); ++i) {
        command += args[i];
        if (i != args.size() - 1) command += " ";
    }
    
    std::string shellPath;
    std::string cmdLine;
    
    // Déterminer le shell et le chemin absolu - REMARQUEZ QUE J'AI RETIRÉ -NoExit
    if (shellType == "--pwsh5") {
        shellPath = "C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe";
        cmdLine = shellPath + " -Command \"" + command + "\"";
    }
    else if (shellType == "--pwsh7") {
        shellPath = "C:\\Program Files\\PowerShell\\7\\pwsh.exe";
        cmdLine = shellPath + " -Command \"" + command + "\"";
    }
    else if (shellType == "--cmd") {
        shellPath = "C:\\Windows\\System32\\cmd.exe";
        cmdLine = shellPath + " /C \"" + command + "\"";
    }
    else {
        std::cerr << "Error: Unsupported shell type '" << shellType << "'" << std::endl;
        return;
    }
    
    std::cout << "Executing: " << cmdLine << std::endl;
    
    // Utiliser CreateProcess au lieu de system()
    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);
    
    // CreateProcess requires a non-const char* that can be modified
    char* cmdLineStr = new char[cmdLine.length() + 1];
    strcpy(cmdLineStr, cmdLine.c_str());
    
    // Create the process
    BOOL success = CreateProcess(
        NULL,               // Application path (NULL uses command line)
        cmdLineStr,         // Command line
        NULL,               // Process security attributes
        NULL,               // Thread security attributes
        FALSE,              // Inherit handles
        0,                  // Creation flags
        NULL,               // Environment
        NULL,               // Current directory
        &si,                // Startup info
        &pi);               // Process information
    
    // Clean up
    delete[] cmdLineStr;
    
    if (!success) {
        DWORD error = GetLastError();
        std::cerr << "Error: CreateProcess failed with code " << error << std::endl;
        return;
    }
    
    // Wait for the process to complete
    WaitForSingleObject(pi.hProcess, INFINITE);
    
    // Get the exit code
    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    
    // Close process and thread handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    
    if (exitCode != 0) {
        std::cerr << "Error: Command execution failed with code " << exitCode << "." << std::endl;
    }
}