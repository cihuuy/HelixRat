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

        // test crypto
        std::string test_message = "Hello, world!";

        Crypto::secret_t *dbstore_secret = new Crypto::secret_t;
        Crypto::create_secret("helixmasta69!", "", *dbstore_secret);
        std::string test_encrypted;

        Crypto::encrypt(test_message, *dbstore_secret, test_encrypted);
        std::string test_decrypted;
        Crypto::decrypt(test_encrypted, *dbstore_secret, test_decrypted);
        if (test_decrypted != test_message)
        {
            return FAIL_INIT_CRYPTO;
        }
        Logger::debug("Crypto test passed");

        // allocate dbstore secret
        bool dbstore_status = DBStore::init(*dbstore_secret);

        if (!dbstore_status)
        {
            return FAIL_INIT_DBSTORE;
        }

        Logger::debug("Initialized DBStore");

        return SUCCESS;
    }
}