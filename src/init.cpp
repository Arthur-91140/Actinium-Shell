#include <iostream>
#include <string>
#include <sstream>
#include <locale>
#include <codecvt>
#include <cstdlib>
#include <windows.h>
#include "./include/command-processor.h"
#include "./include/init.h"

void banner() {
    std::cout << "  ▄▄▄       ▄████▄  ▄▄▄█████▓ ██▓ ███▄    █  ██▓ █    ██  ███▄ ▄███▓    " << std::endl;
    std::cout << "  ▒████▄    ▒██▀ ▀█  ▓  ██▒ ▓▒▓██▒ ██ ▀█   █ ▓██▒ ██  ▓██▒▓██▒▀█▀ ██▒   " << std::endl;
    std::cout << "  ▒██  ▀█▄  ▒▓█    ▄ ▒ ▓██░ ▒░▒██▒▓██  ▀█ ██▒▒██▒▓██  ▒██░▓██    ▓██░   " << std::endl;
    std::cout << "  ░██▄▄▄▄██ ▒▓▓▄ ▄██▒░ ▓██▓ ░ ░██░▓██▒  ▐▌██▒░██░▓▓█  ░██░▒██    ▒██    " << std::endl; 
    std::cout << "   ▓█   ▓██▒▒ ▓███▀ ░  ▒██▒ ░ ░██░▒██░   ▓██░░██░▒▒█████▓ ▒██▒   ░██▒   " << std::endl;
    std::cout << "   ▒▒   ▓▒█░░ ░▒ ▒  ░  ▒ ░░   ░▓  ░ ▒░   ▒ ▒ ░▓  ░▒▓▒ ▒ ▒ ░ ▒░   ░  ░   " << std::endl;
    std::cout << "    ▒   ▒▒ ░  ░  ▒       ░     ▒ ░░ ░░   ░ ▒░ ▒ ░░░▒░ ░ ░ ░  ░      ░   " << std::endl;
    std::cout << "    ░   ▒   ░          ░       ▒ ░   ░   ░ ░  ▒ ░ ░░░ ░ ░ ░      ░      " << std::endl;
    std::cout << "        ░  ░░ ░                ░           ░  ░     ░            ░      " << std::endl;
    std::cout << "            ░                                                           " << std::endl;
}