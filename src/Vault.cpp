#include "Vault.h"

#include <iostream>

namespace fs = std::filesystem;

Vault::Vault(std::string_view masterPassword, const fs::path& filePath)
    : m_vault{}
    , m_masterPassword{ masterPassword }
    , m_filePath{ filePath }
{
    loadFile(m_filePath);
}

void Vault::addEntry(std::string_view source, std::string_view credential, std::string_view password)
{
    m_vault.emplace_back(source, credential, password);
}

void Vault::viewAllEntries() const
{
    for(const PasswordEntry& p : m_vault)
    {
        std::cout << p << "\n";
    }
}

void Vault::saveToFile() const
{
    std::ofstream file(m_filePath);
    file << "source,credential,password\n";
    for (const PasswordEntry& p : m_vault) 
    {
        file << p.source << "," << p.credential << "," << p.password << "\n";
    }
}

void Vault::loadFile(const fs::path& filePath) 
{

    std::ifstream file(filePath);
    if (!file) 
    {
        std::cerr << "Failed to open file for reading--loadFile failed\n";
        return;
    }

    std::string line;
    std::getline(file, line); // Skips the header
    while (std::getline(file, line))
    {
        if (line.empty()) continue;

        std::size_t c1 = line.find(',');
        std::size_t c2 = line.find(',', c1 + 1);

        if (c1 == std::string::npos || c2 == std::string::npos)
        {
            continue;
        }

        std::string source{ line.substr(0, c1) };
        std::string credential{ line.substr(c1 + 1, c2 - (c1 + 1)) };
        std::string password{ line.substr(c2 + 1) };

        m_vault.emplace_back(source, credential, password);
    }
    file.close();
}