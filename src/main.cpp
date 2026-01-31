#include "Vault.h"

#include <vector>

int main()
{
    std::vector<PasswordEntry> passVector{};

    Vault myVault{ passVector, "masterpass" };

    myVault.addEntry("gubzywubzy.com", "Gurvir", "password");
    myVault.viewAllEntries();
}