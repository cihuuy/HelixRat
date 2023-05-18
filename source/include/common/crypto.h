#pragma once

#include <string>

// Path: source/include/crypto.h

// General crypto functions
namespace helixrat::Crypto
{
    struct hash_t
    {
        std::string hash;
    };
    struct secret_t
    {
        uint64_t counter;
        char nonce[16];
        char key[32];
    };
    bool init();
    // Will create a secret_t from a key and a nonce
    bool create_secret(std::string key, std::string nonce, secret_t &secret);
    // Simple. No IV, no salt, no nothing. Just encrypt with key.
    // Returns true if ciphertext is authenticated, false otherwise.
    bool encrypt(std::string message, secret_t &key, std::string &encrypted);
    // Returns true if ciphertext is authenticated, false otherwise.
    bool decrypt(std::string encrypted, secret_t &key, std::string &message);

    // Will hash a message
    void hash(std::string message, hash_t &hash);
    // Will verify a hash. Returns true if hash is verified, false otherwise.
    bool verify_hash(std::string message, hash_t hash);
}