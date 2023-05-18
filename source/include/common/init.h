#pragma once

#include <string>
#include <common/types.h>
#include <common/logger.h>
#include <common/db_store.h>
#include <common/crypto.h>

namespace helixrat {
    error_t init(strings_vector args);
}