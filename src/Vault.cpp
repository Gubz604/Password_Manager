#include "Vault.h"

#include <algorithm>
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
    auto it = findBySource(source);

    if (it == m_vault.end())
        m_vault.emplace_back(source, credential, password);
}

void Vault::deleteEntry(std::string_view source)
{
    auto it = findBySource(source);

    if (it != m_vault.end())
        m_vault.erase(it);
}

void Vault::updateEntry(std::string_view source) 
{
    auto it = findBySource(source);
    PasswordEntry& entry = *it;

    if (it != m_vault.end())
    {
        while(true)
        {
            std::cout 
                << "Update credential or password?\n"
                << "Enter 'c' for credential\n"
                << "Enter 'p' for password\n"
                << "Enter 'q' to return to menu\n";
            

            char choice = getChoice();

            if (choice == 'q') break;

            if (choice == 'c')
            {
                std::cout << "Current Credential is: " << entry.credential << "\n";
                std::cout << "Enter what you want the Credential to be updated to\nCAUTION CHANGES ARE PERMANENT\n> ";
                std::getline(std::cin >> std::ws, entry.credential);
            }

            if (choice == 'p')
            {
                std::cout << "Current Password is: " << entry.password << "\n";
                std::cout << "Enter what you want the Credential to be updated to\nCAUTION CHANGES ARE PERMANENT\n> ";
                std::getline(std::cin >> std::ws, entry.password);
            }
        }
    }
    else 
    {
        std::cout << "No source/site matches in the vault.\n";
        return;
    }
}

void Vault::printEntry(std::string_view source) const
{
    auto it = findBySource(source);

    if (it != m_vault.end())
        std::cout << "Found:\n" << *it << "\n";
    else
        std::cout << "No source/site matches in the vault.\n";
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

std::vector<PasswordEntry>::iterator
Vault::findBySource(std::string_view source)
{
    return std::find_if(m_vault.begin(), m_vault.end(),
        [&](const PasswordEntry& p)
        {
            return p.source == source;
        });
}

std::vector<PasswordEntry>::const_iterator
Vault::findBySource(std::string_view source) const
{
    return std::find_if(m_vault.begin(), m_vault.end(),
        [&](const PasswordEntry& p)
        {
            return p.source == source;
        });
}

char Vault::getChoice()
{
    while(true)
    {
        std::cout << "> ";

        std::string line;
        std::getline(std::cin, line);

        if (!std::cin) return 'q';

        if (line.empty())
        {
            std::cout << "Please enter a choice: | c | p | q |\n";
            continue;
        }

        if (line.size() != 1)
        {
            std::cout << "Please enter a single character.\n";
            continue;
        }

        char c = static_cast<char>(std::tolower(static_cast<unsigned char>(line[0])));
        if (c == 'c' || c == 'p' || c == 'q')
            return c;

        std::cout << "Invalid input. Try again\n";
    }
}