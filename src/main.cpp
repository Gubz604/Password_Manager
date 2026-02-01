#include "Vault.h"
#include "fileIOHandler.h"

#include <limits>
#include <vector>
#include <filesystem> 
#include <fstream>

namespace fs = std::filesystem;

int main()
{
    fileIOHandler fileIOH{};

    if (!fileIOH.checkNameCollision())
        return 1;

    std::cout << "---------------------------------------------------------------------------\n";
    std::cout << "\t\t\tWelcome to Password Manager\n";
    std::cout << "---------------------------------------------------------------------------\n";

    // Check if the directory does not exist
    if (!fileIOH.checkDirectoryExists()) 
    {
        // Create the directory
        if (fileIOH.createDirectory())
        {
            std::cout << "Created directory: /" << fileIOH.getDirectory() << " in:\n" << fs::current_path() << "\n\n";
        }
        else 
        {
            std::cerr << "Failed to create directory: /" << fileIOH.getDirectory() << "\n";
            return 1;
        }
    }
    else 
    {
        std::cout << "Directory: /" << fileIOH.getDirectory() << " found!\n\n";
    }

    while(true) 
    {
        fileIOH.showFiles();

        std::cout 
            << "Choose an existing vault (e) or create a new vault (c)\n"
            << "Enter 'e' to choose an existing vault\n"
            << "Enter 'c' to create a new\n"
            << "Enter 'q' to create quit the program\n";
        char choice{ fileIOH.chooseExistingOrCreateNew() };

        if (choice == 'q')
        {
            std::cout << "Goodbye!\n";
            return 0;
        }

        std::string fileName{};
        if (choice == 'e')
        {
            std::cout << "Enter the EXACT name of the vault you want to enter:\n> ";
            std::cin >> fileName;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // flushes newline so emptyLine check doesnt trigger
            if (!fileIOH.checkPathExists(fileName))
            {
                std::cout << "Vault does not exist in directory\n";
                continue;
            }
        }

        if (choice == 'c')
        {
            std::cout << "Enter the name of the new vault (e.g. Personal, Work)\n> ";
            std::cin >> fileName;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // flushes newline so emptyLine check doesnt trigger
            if (fileIOH.checkPathExists(fileName))
            {
                std::cout << "Vault already exists with that name\n";
                continue;
            }
        }

        fs::path dir = fs::path(fileIOH.getDirectory());

        // strip quotes from input if they're put there
        if (fileName.size() >= 2 && fileName.front() == '"' && fileName.back() == '"')
        {
            fileName = fileName.substr(1, fileName.size() - 2);
        }

        fs::path name = fs::path(fileName);

        // Adds extension if not already given
        if (name.extension() != ".txt")
            name += ".txt";
        
        fs::path fullPath = dir / name;

        Vault currentVault{ fullPath };
        currentVault.vaultMenu();

        break;
    }
}