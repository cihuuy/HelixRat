#pragma once

#include <string>
#include <common/types.h>
#include <common/crypto.h>

// Interface for the DBStore
namespace helixrat::DBStore {
    // Init the DBStore
    error_t init(Crypto::secret_t &secret);
    error_t store(std::string key, std::string value);
    error_t get(std::string key, std::string &value);
    error_t remove(std::string key);
    error_t destroy();
}