#pragma once

#include <string>
#include <common/types.h>
#include <common/crypto.h>

// Interface for the DBStore
namespace helixrat::DBStore {
    // Init the DBStore
    error_t init(Crypto::secret_t &secret);
    bool set(std::string key, std::string value);
    bool get(std::string key, std::string &value);
    bool remove(std::string key);
    bool destroy();
}