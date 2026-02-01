#ifndef FILE_IO_HANDLER_H
#define FILE_IO_HANDLER_H

#include <vector>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

class fileIOHandler
{
    public:
        static constexpr std::string_view VAULT_NAME{ "vaults" };

    private:
        fs::path m_dir{ VAULT_NAME };
        std::vector<fs::path> m_paths{};

    public:    
        fileIOHandler();
    
    std::string_view getDirectory() const;

    bool checkNameCollision() const;

    bool checkDirectoryExists() const;

    bool createDirectory() const;

    bool checkFileExists(const fs::path& file) const;

    void createFile(const fs::path& file);

    void showFiles() const;

    char chooseExistingOrCreateNew() const;

    bool checkPathExists(std::string_view file) const;

    private:
    void loadFilePaths();

    std::vector<fs::path>::const_iterator findPath(std::string_view file) const;

    std::vector<fs::path>::iterator findPath(std::string_view file);
};

#endif