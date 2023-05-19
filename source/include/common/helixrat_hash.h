#pragma once

#include <string>

namespace helixrat::hash
{
    std::string helixrat_hash(uint8_t *data, uint64_t length);
    std::string helixrat_hashx(uint8_t *data, uint64_t length);
    uint64_t helixrat_check(uint8_t *data, uint64_t length);

}