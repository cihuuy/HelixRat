#include <common/init.h>

namespace helixrat
{
    error_t init(strings_vector args)
    {
        bool init_status = Logger::init();
        if (!init_status)
        {
            return FAIL_INIT_LOGGER;
        }

        Logger::debug("Initialized logger");

        bool crypto_status = Crypto::init();
        if (!crypto_status)
        {
            return FAIL_INIT_CRYPTO;
        }

        Crypto::secret_t dbstore_secret;
        Crypto::create_secret("password1", "", dbstore_secret);
        bool dbstore_status = DBStore::init(dbstore_secret);

        if (!dbstore_status)
        {
            return FAIL_INIT_DBSTORE;
        }

        Logger::debug("Initialized DBStore");

        return SUCCESS;
    }
}