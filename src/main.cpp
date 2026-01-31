#include "Vault.h"

#include <vector>
#include <filesystem> 
#include <fstream>

namespace fs = std::filesystem;

int main()
{
    // std::cout << fs::current_path() << "\n"; // Check what the working directory is

    fs::path dir = "vault";
    fs::path file = dir / "vault.txt";

    // In case something named "vault" exists but it's not a directory
    if (fs::exists(dir) && !fs::is_directory(dir))
    {
        std::cerr <<  dir << " exists but is not a directory.\n";
        return 1;
    }

    // Check if directory does not exist
    if (!fs::exists(dir)) 
    {
        // Create the directory
        if (fs::create_directories(dir))
        {
            std::cout << "Created directory: " << dir << "\n";
            std::ofstream f(file);  // Create the file
            if (!f)
            {
                std::cerr << "Failed to create a file\n";
                return 1;
            }
        }
        else 
        {
            std::cerr << "Failed to create directory: " << dir << "\n";
        }
    }
    else // The directory does exist
    {
        // Check if the file exists
        if (!fs::exists(file)) 
        {
            // Create file
            std::ofstream f(file);
            if (!f)
            {
                std::cerr << "Failed to create a file\n";
                return 1;
            }
            std::cout << "File created\n";
        }
        else
        {
            std::cout << "File already exists\n";
        }
    }

    std::vector<PasswordEntry> passVector{};

    Vault myVault{ passVector, "masterpass" };

    myVault.addEntry("gubzywubzy.com", "Gurvir", "password");
    myVault.viewAllEntries();
}