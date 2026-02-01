#include "fileIOHandler.h"

#include <algorithm>
#include <iostream>

namespace fs = std::filesystem;

fileIOHandler::fileIOHandler()
{
    loadFilePaths();
}

std::string_view fileIOHandler::getDirectory() const { return VAULT_NAME; }

bool fileIOHandler::checkNameCollision() const
{
    // In case something named "vault" exists but it's not a directory
    if (fs::exists(m_dir) && !fs::is_directory(m_dir))
    {
        std::cerr 
            <<  m_dir 
            << " exists but is not a directory.\n"
            << "This program requires NO files/directories be name " 
            << "'" << m_dir << "'"
            << " in the root folder";
        return false;
    }
    return true;
}

bool fileIOHandler::checkDirectoryExists() const { return fs::exists(m_dir); }

bool fileIOHandler::createDirectory() const { return fs::create_directories(m_dir); }

bool fileIOHandler::checkFileExists(const fs::path& file) const { return fs::exists(file); }

void fileIOHandler::createFile(const fs::path& file)
{
    std::ofstream f(file);  // Create the file
                if (!f)
                {
                    std::cerr << "Failed to create a file\n";
                    return;
                }
                f << "source,credential,password\n";
}

void fileIOHandler::showFiles() const
{
    std::cout << "\t\t\tList of Current Vaults\n";
    std::cout << "---------------------------------------------------------------------------\n";
    for (const fs::path& p : m_paths)
    {
        std::cout << p.filename() << "\n";
    }
    std::cout << "\n";
}

char fileIOHandler::chooseExistingOrCreateNew() const
{
    while(true)
    {
        std::cout << "> ";

        std::string line;
        std::getline(std::cin, line);

        if (!std::cin) return 'q';

        if (line.empty())
        {
            std::cout << "Please enter a choice: | e | c | q |\n";
            continue;
        }

        if (line.size() != 1)
        {
            std::cout << "Please enter a single character.\n";
            continue;
        }

        char c = static_cast<char>(std::tolower(static_cast<unsigned char>(line[0])));
        if (c == 'e' || c == 'c' || c == 'q')
            return c;

        std::cout << "Invalid input. Try again\n";
    }
}

bool fileIOHandler::checkPathExists(std::string_view file) const
{
    auto it = findPath(file);
    return (it != m_paths.end());
}

void fileIOHandler::loadFilePaths()
{
    if (!fs::exists(m_dir) || !fs::is_directory(m_dir))
    {
        std::cerr << "Not a directory: " << m_dir << "\n";
        return;
    }

    for (const auto& entry : fs::directory_iterator(m_dir))
    {
        if (entry.is_regular_file())
        {
            m_paths.push_back(entry.path());
        }
    }
}

std::vector<fs::path>::const_iterator
fileIOHandler::findPath(std::string_view file) const
{
    fs::path filePath(file);
    return std::find_if(m_paths.begin(), m_paths.end(), 
        [&](const fs::path& p)
    {
        return p.filename() == filePath;
    });
}

std::vector<fs::path>::iterator
fileIOHandler::findPath(std::string_view file)
{
    fs::path filePath(file);
    return std::find_if(m_paths.begin(), m_paths.end(), 
        [&](const fs::path& p)
    {
        return p.filename() == filePath;
    });
}