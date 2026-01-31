#include "Vault.h"

#include <iostream>

Vault::Vault(const std::vector<PasswordEntry>& vault, std::string_view masterPassword)
    : m_vault{ vault }
    , m_masterPassword{ masterPassword }
{}

void Vault::addEntry(std::string_view source, std::string_view credential, std::string_view password)
{
    m_vault.emplace_back(source, credential, password);
}

void Vault::viewAllEntries() const
{
    for(PasswordEntry p : m_vault)
    {
        std::cout << p << "\n";
    }
}