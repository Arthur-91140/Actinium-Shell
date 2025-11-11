#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <ctime>

// Command: date
// Display the current date and time

extern "C" __declspec(dllexport) void execute(const std::vector<std::string>& args) {
    SYSTEMTIME st;
    GetLocalTime(&st);

    // Day of week names
    const char* dayNames[] = {
        "Sunday", "Monday", "Tuesday", "Wednesday",
        "Thursday", "Friday", "Saturday"
    };

    // Month names
    const char* monthNames[] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };

    std::cout << dayNames[st.wDayOfWeek] << ", "
              << monthNames[st.wMonth - 1] << " "
              << st.wDay << ", "
              << st.wYear << " "
              << (st.wHour < 10 ? "0" : "") << st.wHour << ":"
              << (st.wMinute < 10 ? "0" : "") << st.wMinute << ":"
              << (st.wSecond < 10 ? "0" : "") << st.wSecond
              << "\n";
}
