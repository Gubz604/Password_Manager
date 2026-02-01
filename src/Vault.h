#ifndef VAULT_H
#define VAULT_H

#include "PasswordEntry.h"

#include <optional>
#include <filesystem>
#include <fstream>
#include <string_view>
#include <vector>

namespace fs = std::filesystem;

class Vault 
{
    private:
        std::vector<PasswordEntry> m_vault{};
        std::string m_masterPassword{};
        const fs::path& m_filePath{};

    public:
        Vault(const fs::path& filePath, std::string_view masterPassword="default");

    void addEntry();

    void deleteEntry();

    void updateEntry();

    void printEntry(std::string_view source) const;

    void viewAllEntries() const;

    void vaultMenu();

    void saveToFile() const;

    private:
    void loadFile(const fs::path& filePath);

    std::vector<PasswordEntry>::const_iterator findBySource(std::string_view source) const;

    std::vector<PasswordEntry>::iterator findBySource(std::string_view source);

    char getChoiceUpdate();

    int getChoiceMenu();

    std::string getChoiceEntryValue() const;
};

#endif 