#include <iostream>
#include <string>
#include <sstream>
#include <locale>
#include <codecvt>
#include <cstdlib>
#include <windows.h>
#include <shlobj.h> // For SHGetFolderPath

void banner();

std::string GetUserFolderPath();