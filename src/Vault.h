#ifndef VAULT_H
#define VAULT_H

#include "PasswordEntry.h"

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
        Vault(std::string_view masterPassword, const fs::path& filePath);

    void addEntry(std::string_view source, std::string_view credential, std::string_view password);

    void viewAllEntries() const;

    bool searchSource(std::string_view source) const;

    void saveToFile() const;

    private:
    void loadFile(const fs::path& filePath);
};

#endif 