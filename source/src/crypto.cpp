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

    bool create_secret(std::string password, std::string salt, secret_t &secret)
    {
        secret.counter = 0;
        hash_t hash;
        if (password.length() < 8)
        {
            return false;
        }
        Crypto::hash(password, hash);
        memcpy(secret.key, hash.hash.c_str(), 32);
        memcpy(secret.nonce, hash.hash.c_str(), 16);

        return true;
    }

    bool encrypt(std::string plaintext, secret_t &secret, std::string &ciphertext)
    {
        chacha20_context ctx;
        ciphertext = plaintext;
        chacha20_init_context(&ctx, (uint8_t *)secret.key, (uint8_t *)secret.nonce, secret.counter);
        chacha20_xor(&ctx, (uint8_t *)ciphertext.c_str(), ciphertext.length());

        return true;
    }

    bool decrypt(std::string ciphertext, secret_t &secret, std::string &plaintext)
    {
        chacha20_context ctx;
        plaintext = ciphertext;
        chacha20_init_context(&ctx, (uint8_t *)secret.key, (uint8_t *)secret.nonce, secret.counter);
        chacha20_xor(&ctx, (uint8_t *)plaintext.c_str(), plaintext.length());
        return true;
    }

    void hash(std::string message, hash_t &hash)
    {
        hash.hash = hash::helixrat_hash((uint8_t *)message.c_str(), message.length());
    }

    bool verify_hash(std::string message, hash_t hash)
    {
        hash_t new_hash;
        Crypto::hash(message, new_hash);
        return new_hash.hash == hash.hash;
    }

}