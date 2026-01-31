#include "Vault.h"

#include <vector>
#include <filesystem> 

int main()
{
    std::vector<PasswordEntry> passVector{};

    Vault myVault{ passVector, "masterpass" };

    myVault.addEntry("gubzywubzy.com", "Gurvir", "password");
    myVault.viewAllEntries();
}