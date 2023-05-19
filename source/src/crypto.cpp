#include <common/crypto.h>
#include <common/chacha20.h>
#include <common/helixrat_hash.h>
#include <string.h>

namespace helixrat::Crypto
{
    bool init()
    {
        return true;
    }

    bool create_secret(std::string password, std::string nonce, secret_t &secret)
    {
        secret.counter = 0;
        hash_t hash;
        if (password.length() < 8)
        {
            return false;
        }
        Crypto::hash(password + nonce, hash);
        memcpy(secret.key, hash.hash.c_str(), 32);
        memcpy(secret.nonce, hash.hash.c_str(), 16);

        return true;
    }
    // Note this is not secure for common use.
    // As long as we dont store exactly the same data, it should be fine.
    // To store exactly the same data, generate a new secret.

    bool encrypt(const std::string plaintext, secret_t &secret, std::string &ciphertext)
    {
        chacha20_context ctx;
        ciphertext.resize(plaintext.length()); // Make sure ciphertext has enough space
        memcpy((uint8_t *)ciphertext.data(), (uint8_t *)plaintext.c_str(), plaintext.length());
        chacha20_init_context(&ctx, (uint8_t *)secret.key, (uint8_t *)secret.nonce, 0);
        chacha20_xor(&ctx, (uint8_t *)ciphertext.data(), ciphertext.length());

        return true;
    }

    bool decrypt(std::string ciphertext, secret_t &secret, std::string &plaintext)
    {
        chacha20_context ctx;
        plaintext = ciphertext;
        chacha20_init_context(&ctx, (uint8_t *)secret.key, (uint8_t *)secret.nonce, 0);
        chacha20_xor(&ctx, (uint8_t *)plaintext.data(), plaintext.length());
        return true;
    }

    void hash(std::string message, hash_t &hash)
    {
        hash.hash = hash::helixrat_hash((uint8_t *)message.c_str(), message.length());
    }

    uint64_t check(std::string message)
    {
        return hash::helixrat_check((uint8_t *)message.c_str(), message.length());
    }

    void hashx(std::string message, std::string &hashHex)
    {
        hashHex = hash::helixrat_hashx((uint8_t *)message.c_str(), message.length());
    }

    bool verify_hash(std::string message, hash_t hash)
    {
        hash_t new_hash;
        Crypto::hash(message, new_hash);
        return new_hash.hash == hash.hash;
    }

}