#include "Crypto.h"
#include <sodium.h>

#include <array>
#include <cstring>
#include <stdexcept>

namespace crypto
{
    namespace
    {
        constexpr char MAGIC[5] = {'V', 'A', 'U', 'L', 'T'};
        constexpr std::uint8_t VERSION = 1;

        // helper: append POD bytes
        template <typename T>
        void append(Bytes &out, const T &value)
        {
            const auto *p = reinterpret_cast<const std::uint8_t *>(&value);
            out.insert(out.end(), p, p + sizeof(T));
        }

        // helper: read POD bytes
        template <typename T>
        T read(const Bytes &in, std::size_t &offset)
        {
            if (offset + sizeof(T) > in.size())
                throw std::runtime_error("Corrupt vault (truncated)");
            T v{};
            std::memcpy(&v, in.data() + offset, sizeof(T));
            offset += sizeof(T);
            return v;
        }

        void appendRaw(Bytes &out, const std::uint8_t *data, std::size_t n)
        {
            out.insert(out.end(), data, data + n);
        }

        void readRaw(const Bytes &in, std::size_t &offset, std::uint8_t *dst, std::size_t n)
        {
            if (offset + n > in.size())
                throw std::runtime_error("Corrupt vault (truncated)");
            std::memcpy(dst, in.data() + offset, n);
            offset += n;
        }

        // Not used, but maybe useful later
        // static std::vector<std::uint8_t> toBytes(const std::string &s)
        // {
        //     return {s.begin(), s.end()};
        // }

        // static std::string toString(const std::vector<std::uint8_t> &b)
        // {
        //     return std::string(b.begin(), b.end());
        // }

    }

    Bytes encryptVault(const Bytes &plaintext, const std::string &masterPassword)
    {
        if (sodium_init() < 0)
            throw std::runtime_error("libsodium init failed");

        // KDF params (interactive is fine for a personal project; can upgrade later)
        const std::uint64_t opslimit = crypto_pwhash_OPSLIMIT_INTERACTIVE;
        const std::size_t memlimit = crypto_pwhash_MEMLIMIT_INTERACTIVE;

        std::array<std::uint8_t, crypto_pwhash_SALTBYTES> salt{};
        std::array<std::uint8_t, crypto_secretbox_NONCEBYTES> nonce{};
        randombytes_buf(salt.data(), salt.size());
        randombytes_buf(nonce.data(), nonce.size());

        std::array<std::uint8_t, crypto_secretbox_KEYBYTES> key{};
        if (crypto_pwhash(
                key.data(), key.size(),
                masterPassword.c_str(), masterPassword.size(),
                salt.data(),
                opslimit, memlimit,
                crypto_pwhash_ALG_ARGON2ID13) != 0)
        {
            throw std::runtime_error("Out of memory during key derivation");
        }

        Bytes ciphertext(plaintext.size() + crypto_secretbox_MACBYTES);
        crypto_secretbox_easy(
            ciphertext.data(),
            plaintext.data(), plaintext.size(),
            nonce.data(),
            key.data());

        sodium_memzero(key.data(), key.size());

        // Build blob: MAGIC + VERSION + opslimit + memlimit + salt + nonce + ciphertext
        Bytes blob;
        blob.reserve(5 + 1 + sizeof(opslimit) + sizeof(memlimit) + salt.size() + nonce.size() + ciphertext.size());

        appendRaw(blob, reinterpret_cast<const std::uint8_t *>(MAGIC), 5);
        append(blob, VERSION);
        append(blob, opslimit);
        append(blob, memlimit);
        appendRaw(blob, salt.data(), salt.size());
        appendRaw(blob, nonce.data(), nonce.size());
        appendRaw(blob, ciphertext.data(), ciphertext.size());

        return blob;
    }

    Bytes decryptVault(const Bytes &blob, const std::string &masterPassword)
    {
        if (sodium_init() < 0)
            throw std::runtime_error("libsodium init failed");

        std::size_t off = 0;

        // Check magic
        if (blob.size() < 6)
            throw std::runtime_error("Corrupt vault");
        if (std::memcmp(blob.data(), MAGIC, 5) != 0)
            throw std::runtime_error("Not a vault file");
        off += 5;

        const auto version = read<std::uint8_t>(blob, off);
        if (version != VERSION)
            throw std::runtime_error("Unsupported vault version");

        const auto opslimit = read<std::uint64_t>(blob, off);
        const auto memlimit = read<std::size_t>(blob, off);

        std::array<std::uint8_t, crypto_pwhash_SALTBYTES> salt{};
        std::array<std::uint8_t, crypto_secretbox_NONCEBYTES> nonce{};

        readRaw(blob, off, salt.data(), salt.size());
        readRaw(blob, off, nonce.data(), nonce.size());

        if (off > blob.size())
            throw std::runtime_error("Corrupt vault");

        const std::size_t clen = blob.size() - off;
        if (clen < crypto_secretbox_MACBYTES)
            throw std::runtime_error("Corrupt vault");

        std::array<std::uint8_t, crypto_secretbox_KEYBYTES> key{};
        if (crypto_pwhash(
                key.data(), key.size(),
                masterPassword.c_str(), masterPassword.size(),
                salt.data(),
                opslimit, memlimit,
                crypto_pwhash_ALG_ARGON2ID13) != 0)
        {
            throw std::runtime_error("Out of memory during key derivation");
        }

        Bytes plaintext(clen - crypto_secretbox_MACBYTES);

        const int ok = crypto_secretbox_open_easy(
            plaintext.data(),
            blob.data() + off, clen,
            nonce.data(),
            key.data());

        sodium_memzero(key.data(), key.size());

        if (ok != 0)
        {
            throw std::runtime_error("Wrong password or vault was modified");
        }

        return plaintext;
    }
}