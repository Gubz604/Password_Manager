#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace crypto
{
    using Bytes = std::vector<std::uint8_t>;

    // Encrypt plaintext bytes using master password.
    // Returns a self-contained blob you can write to disk.
    Bytes encryptVault(const Bytes& plaintext, const std::string& masterPassword);

    // Decrypt blob using master password.
    // Throws std::runtime_error on failure (wrong password or tampered).
    Bytes decryptVault(const Bytes& blob, const std::string& masterPassword);
}
