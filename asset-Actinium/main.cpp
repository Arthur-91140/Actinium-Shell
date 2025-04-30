#include <windows.h>
#include <iostream>

bool CreateRegistryKey(HKEY root, const std::string& subKey, const std::string& valueName, const std::string& valueData) {
    HKEY hKey;
    LONG result = RegCreateKeyExA(
        root,
        subKey.c_str(),
        0,
        nullptr,
        REG_OPTION_NON_VOLATILE,
        KEY_WRITE,
        nullptr,
        &hKey,
        nullptr
    );

    if (result != ERROR_SUCCESS) {
        std::cerr << "Échec de création de la clé : " << subKey << std::endl;
        return false;
    }

    result = RegSetValueExA(
        hKey,
        valueName.empty() ? nullptr : valueName.c_str(),
        0,
        REG_SZ,
        reinterpret_cast<const BYTE*>(valueData.c_str()),
        static_cast<DWORD>(valueData.size() + 1)
    );

    RegCloseKey(hKey);

    if (result != ERROR_SUCCESS) {
        std::cerr << "Échec d'écriture dans la clé : " << subKey << std::endl;
        return false;
    }

    return true;
}

int main() {
    const std::string extension = ".acsh";
    const std::string fileType = "ActiniumScriptFile";
    const std::string iconPath = "\"C:\\Program Files\\Actinium\\resources\\acsh-icon.ico\""; // <- modif ici
    const std::string executablePath = "\"C:\\Program Files\\Actinium\\actinium.exe\" \"%1\"";

    // Associer .acsh à un type de fichier personnalisé
    CreateRegistryKey(HKEY_CURRENT_USER, "Software\\Classes\\" + extension, "", fileType);

    // Définir le nom du type de fichier
    CreateRegistryKey(HKEY_CURRENT_USER, "Software\\Classes\\" + fileType, "", "Fichier script Actinium");

    // Définir l'icône
    CreateRegistryKey(HKEY_CURRENT_USER, "Software\\Classes\\" + fileType + "\\DefaultIcon", "", iconPath);

    // Définir le programme
    CreateRegistryKey(HKEY_CURRENT_USER, "Software\\Classes\\" + fileType + "\\shell\\open\\command", "", executablePath);

    std::cout << "Association .acsh créée avec succès." << std::endl;
    return 0;
}
