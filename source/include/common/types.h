#pragma once

#include <vector>
#include <string>

namespace helixrat
{
    typedef std::vector<std::string> strings_vector;
    enum HEXLIX_ERR
    {
        SUCCESS = 1,
        FAIL_INIT_LOGGER = 20,
        FAIL_INIT_DBSTORE,
        FAIL_INIT_ERROR,
        FILE_OPEN_ERROR,
        FAIL_INIT_CRYPTO,
        FAIL_INVALID_DBSTORE,
        FAIL_LENGTH_TOO_LONG,
    };

    typedef HEXLIX_ERR error_t;


}