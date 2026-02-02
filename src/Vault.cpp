#include "Vault.h"
#include "Crypto.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

namespace fs = std::filesystem;

Vault::Vault(const fs::path& filePath, std::string_view masterPassword)
    : m_vault{}
    , m_masterPassword{ masterPassword }
    , m_filePath{ filePath }
{
    loadFile(m_filePath);
}

void Vault::addEntry()
{
    std::cout << "Enter the name of the source/site:\n";
    std::string source{ getChoiceEntryValue() };

    auto it = findBySource(source);

    if (it == m_vault.end())
    {
        std::cout << "Enter the name of the credential (e.g. username)\n";
        std::string credential{ getChoiceEntryValue() };
        std::cout << "Enter your password\n";
        std::string password{ getChoiceEntryValue() };
        m_vault.emplace_back(source, credential, password);
    }
    else
        std::cout << "Source/site already exists... choose to update instead\n";
}

void Vault::deleteEntry()
{
    std::cout << "Enter the name of the source/site:\n> ";
    std::string source{ getChoiceEntryValue() };
    auto it = findBySource(source);

    if (it != m_vault.end())
    {
        m_vault.erase(it);
        std::cout << "Entry deleted successfully!\n";
    }
}

void Vault::updateEntry() 
{
    std::cout << "Enter the name of the source/site:\n";
    std::string source{ getChoiceEntryValue() };
    auto it = findBySource(source);
    if (it == m_vault.end())
    {
        std::cout << "No source/site mathes in the vault.\n";
        return;
    }

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
            

            char choice = getChoiceUpdate();

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

void Vault::vaultMenu() 
{
    std::cout << "\t\t\tVault Menu for " << m_filePath.filename() << "\n";
    std::cout << "---------------------------------------------------------------------------\n";

    while(true)
    {
        std::cout
            << "Enter the number from the following menu:\n"
            << "1. Add Entry\n"
            << "2. Delete Entry\n"
            << "3. Update Entry\n"
            << "4. View All Entries\n"
            << "5. Quit Vault Menu and Save Changes\n"
            ;
        int choice{ getChoiceMenu() };

        switch (choice)
        {
        case 1:
            addEntry();
            break;
        case 2:
            deleteEntry();
            break;
        case 3:
            updateEntry();
            break;
        case 4:
            viewAllEntries();
            break;
        case 5:
            saveToFile();
            return;
        default:
            return;
        }
    }
}

void Vault::saveToFile() const
{
    std::cout << "SAVING TO: " << fs::absolute(m_filePath) << "\n";

    std::ostringstream oss;
    oss << "source,credential,password\n";
    for (const PasswordEntry& p : m_vault)
        oss << p.source << "," << p.credential << "," << p.password << "\n";
    
    const std::string csv = oss.str();

    // Encrypt
    const auto blob = crypto::encryptVault(
        std::vector<std::uint8_t>(csv.begin(), csv.end()),
        m_masterPassword
    );

    // Write encrypted bytes as binary
    std::ofstream file(m_filePath, std::ios::binary);
    if (!file)
    {
        std::cerr << "Failed to open file for writing: " << m_filePath << "\n";
        return;
    }

    file.write(reinterpret_cast<const char*>(blob.data()),
                static_cast<std::streamsize>(blob.size()));

    if (!file)
        std::cerr << "Write failed: " << m_filePath << "\n";
}

void Vault::loadFile(const fs::path& filePath) 
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file)
    {
        std::cerr << "Failed to open file for reading -- loadFile failed: " << filePath << "\n";
        return;
    }

    // Read whole file into blob
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size <= 0)
    {
        // Empty vault file is fine - treat as no entries
        return;
    }

    std::vector<std::uint8_t> blob(static_cast<std::size_t>(size));
    if (!file.read(reinterpret_cast<char*>(blob.data()), size))
    {
        std::cerr << "Failed to read vault file: " << filePath << "\n";
        return;
    }

    // Decrypt
    std::string csv;
    try
    {
        const auto plaintext = crypto::decryptVault(blob, m_masterPassword);
        csv.assign(plaintext.begin(), plaintext.end());
    }
    catch (const std::exception& e)
    {
        std::cerr << "Vault decrypt failed: " << e.what() << "\n";
        return;
    }

    // Parse CSV 
    std::istringstream iss(csv);

    std::string line;
    std::getline(iss, line); // skip header

    while (std::getline(iss, line))
    {
        if (line.empty()) continue;

        std::size_t c1 = line.find(',');
        std::size_t c2 = line.find(',', c1 + 1);

        if (c1 == std::string::npos || c2 == std::string::npos)
            continue;

        std::string source{ line.substr(0, c1) };
        std::string credential{ line.substr(c1 + 1, c2 - (c1 + 1)) };
        std::string password{ line.substr(c2 + 1) };

        m_vault.emplace_back(source, credential, password);
    }
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

char Vault::getChoiceUpdate()
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

int Vault::getChoiceMenu()
{
    while(true)
    {
        std::cout << "> ";

        std::string line;
        std::getline(std::cin, line);

        if (!std::cin) return -1;

        if (line.empty())
        {
            std::cout << "Please enter a choice: | 1 | 2 | 3 | 4 | 5 |\n";
            continue;
        }

        if (line.size() != 1)
        {
            std::cout << "Please enter an integer.\n";
            continue;
        }

        int c = line[0] - '0';
        if (c >- 1 && c <= 5)
            return c;

        std::cout << "Invalid input. Try again\n";
    }
}

std::string Vault::getChoiceEntryValue() const
{
    while(true)
    {
        std::cout << "> ";

        std::string line;
        std::getline(std::cin, line);

        if (!std::cin) return {};

        if (line.empty())
        {
            std::cout << "Please enter a name\n";
            continue;
        }

        return line;
    }
}