#ifndef VAULT_H
#define VAULT_H

#include "PasswordEntry.h"

#include <string_view>
#include <vector>

class Vault 
{
    private:
        std::vector<PasswordEntry> m_vault{};
        std::string m_masterPassword{};

    public:
        Vault(const std::vector<PasswordEntry>& vault, std::string_view masterPassword);

    void addEntry(std::string_view source, std::string_view credential, std::string_view password);

    void viewAllEntries() const;
};

#endif 