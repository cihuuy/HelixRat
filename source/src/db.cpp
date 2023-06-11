#include <common/db.h>
#include <common/logger.h>
#include <common/crypto.h>
#include <fstream>
#include <string.h>
#include <filesystem>
#include <sstream>
#include <iomanip>

#define DEFAULT_DBSTORE_DIRECTORY "~/.local/share/messenger"

using namespace helixrat;

namespace helixrat::DBStore
{
    static Crypto::secret_t *dbstore_secret;
    static std::string dbstore_directory;
#define DBSTORE_VERSION 0x02
#define DBSTORE_MAXSIZE 4096

    // disguise as other file type
    struct db_store_mask_t
    {
        char magic_mask[16];
        uint16_t magic;
    } __attribute__((packed));

    // This is encrypted
    struct db_store_header_t
    {
        uint16_t version;
        uint32_t payload_size;
        uint64_t checksum;
    } __attribute__((packed));

    // Mask as a Sqlite3 database
    db_store_mask_t init_header()
    {
        db_store_mask_t header;
        uint8_t magic[] = {0x53, 0x51, 0x4c, 0x69, 0x74, 0x65, 0x20, 0x66, 0x6f, 0x72, 0x6d, 0x61, 0x74, 0x20, 0x33, 0x00};
        memcpy(header.magic_mask, magic, sizeof(header.magic_mask));
        header.magic = 0x8001;
        return header;
    }

    bool make_header_encrypted(uint32_t payload_size, db_store_header_t &header_encrypted)
    {
        db_store_header_t header;

        header.version = DBSTORE_VERSION;
        header.payload_size = payload_size;
        header.checksum = 0x00;

        header.checksum = Crypto::check(std::string((char *)&header, sizeof(header)));
        const std::string plaintext = std::string((char *)&header, sizeof(db_store_header_t));
        std::string ciphertext;
        Crypto::encrypt(plaintext, *dbstore_secret, ciphertext);

        memcpy(&header_encrypted, ciphertext.c_str(), sizeof(header_encrypted));

        return true;
    }

    bool init_dbstore()
    {
        Logger::debug("DBStore init_dbstore called");
        // create directory if it doesn't exist
        if (!std::filesystem::exists(dbstore_directory) && !std::filesystem::create_directories(dbstore_directory))
        {
            Logger::error("DBStore failed to create directory");
            return false;
        }
        // set permissions to 700
        std::filesystem::permissions(dbstore_directory, std::filesystem::perms::owner_all);

        if (std::filesystem::exists(dbstore_directory + "/index.db"))
        {
            // set permissions to 600
            std::filesystem::permissions(dbstore_directory + "/index.db", std::filesystem::perms::owner_read | std::filesystem::perms::owner_write);
            return true;
        }
        // create file if it doesn't exist
        std::fstream file(dbstore_directory + "/index.db", std::ios::in | std::ios::out | std::ios::binary | std::ios::app);
        if (!file.is_open())
        {
            Logger::error("DBStore failed to open file");
            return false;
        }
        // set permissions to 600
        std::filesystem::permissions(dbstore_directory + "/index.db", std::filesystem::perms::owner_read | std::filesystem::perms::owner_write);

        // write header
        db_store_mask_t header = init_header();
        file.write((char *)&header, sizeof(header));
        file.close();

        return true;
    }

    bool verify_header(std::fstream &file)
    {
        Logger::debug("DBStore verify_header called");
        if (!file.is_open())
        {
            Logger::error("DBStore file not open");
            return false;
        }
        db_store_mask_t header;
        file.seekg(0, std::ios::beg);
        file.read((char *)&header, sizeof(header));
        if (header.magic != 0x8001)
        {
            Logger::error("DBStore header magic mismatch");
            return false;
        }
        if (memcmp(header.magic_mask, init_header().magic_mask, sizeof(header.magic_mask)) != 0)
        {
            Logger::error("DBStore header magic mask mismatch");
            return false;
        }
        // If the file is empty, it's a valid header
        if (file.peek() == std::ifstream::traits_type::eof())
        {
            return true;
        }

        // Read encrypted header
        db_store_header_t header_encrypted;
        file.read((char *)&header_encrypted, sizeof(header_encrypted));
        std::string plaintext;
        // Decrypt header
        Crypto::decrypt(std::string((char *)&header_encrypted, sizeof(header_encrypted)), *dbstore_secret, plaintext);
        memcpy(&header_encrypted, plaintext.c_str(), sizeof(header_encrypted));

        // Verify checksum
        uint64_t checkum = header_encrypted.checksum;
        header_encrypted.checksum = 0x00;
        if (checkum != Crypto::check(std::string((char *)&header_encrypted, sizeof(header_encrypted))))
        {
            Logger::error("DBStore header checksum mismatch");
            return false;
        }

        // Verify version
        if (header_encrypted.version != DBSTORE_VERSION)
        {
            Logger::error("DBStore header version mismatch");
            return false;
        }

        // Verify payload size
        if (header_encrypted.payload_size > DBSTORE_MAXSIZE)
        {
            Logger::error("DBStore header payload size too large");
            return false;
        }

        return true;
    }
    error_t init(Crypto::secret_t &secret)
    {
        Logger::debug("DBStore init called");
        dbstore_secret = &secret;

        // resolve home directory for dbstore
        const char *homeEnv = getenv("HOME");
        if (!homeEnv)
        {
            Logger::error("DBStore failed to resolve home directory");
            return FAIL_INIT_DBSTORE;
        }

        std::filesystem::path home = std::filesystem::path(homeEnv);
        dbstore_directory = DEFAULT_DBSTORE_DIRECTORY;
        dbstore_directory.replace(dbstore_directory.find("~"), 1, home.string());

        if (!init_dbstore())
        {
            Logger::error("DBStore init_dbstore failed");
            return FAIL_INIT_DBSTORE;
        }

        std::fstream file(dbstore_directory + "/index.db", std::ios::in | std::ios::out | std::ios::binary | std::ios::app);
        if (!verify_header(file))
        {
            Logger::error("DBStore verify_header failed");
            return FAIL_INIT_DBSTORE;
        }

        file.close();

        return SUCCESS;
    }

    bool set(std::string key, std::string value)
    {
        Logger::debug("DBStore store called");
        // Store file in dbstore_directory
        // Filename = hash(key)[:10]
        // File contents = header_mask + header + value

        // Create file
        std::string hashHex;
        Crypto::hashx(key, hashHex);

        std::string filename = hashHex.substr(0, 10) + ".db";

        std::ofstream file(dbstore_directory + "/" + filename, std::ios::out | std::ios::trunc);
        if (!file.is_open())
        {
            Logger::error("DBStore failed to open file");
            return false;
        }

        // Write header
        db_store_mask_t header = init_header();
        file.write((char *)&header, sizeof(header));

        // Write encrypted header
        db_store_header_t header_encrypted;
        if (!make_header_encrypted(value.length(), header_encrypted))
        {
            Logger::error("DBStore make_header_encrypted failed");
            return false;
        }
        file.write((char *)&header_encrypted, sizeof(header_encrypted));

        // Write value encrypted
        std::string value_encrypted;
        if (!Crypto::encrypt(value, *dbstore_secret, value_encrypted))
        {
            Logger::error("DBStore Crypto::encrypt failed");
            return false;
        }
        file.write(value_encrypted.c_str(), value_encrypted.length());

        file.close();

        return true;
    }

    bool get(std::string key, std::string &value)
    {
        Logger::debug("DBStore get called");
        // Get file value in dbstore_directory
        // Filename = hash(key)[:10]
        // File contents = header_mask + header + value
        // Extract value from file

        std::string hashHex;
        Crypto::hashx(key, hashHex);

        std::string filename = hashHex.substr(0, 10) + ".db";

        std::fstream file(dbstore_directory + "/" + filename, std::ios::in);
        if (!file.is_open())
        {
            Logger::error("DBStore failed to open file");
            return false;
        }

        // Verify header
        if (!verify_header(file))
        {
            Logger::error("DBStore verify_header failed");
            return false;
        }

        // Header is valid, read value
        file.seekg(sizeof(db_store_mask_t) + sizeof(db_store_header_t), std::ios::beg);
        std::string value_encrypted;

        file.seekg(0, std::ios::end);
        size_t file_size = file.tellg();
        file.seekg(sizeof(db_store_mask_t) + sizeof(db_store_header_t), std::ios::beg);
        size_t value_size = file_size - (sizeof(db_store_mask_t) + sizeof(db_store_header_t));

        if (value_size >= DBSTORE_MAXSIZE)
        {
            Logger::error("DBStore value size too large");
            return false;
        }

        value_encrypted.resize(value_size);

        file.read(&value_encrypted[0], value_size);

        // Decrypt value
        if (!Crypto::decrypt(value_encrypted, *dbstore_secret, value))
        {
            Logger::error("DBStore decrypt failed");
            return false;
        }

        file.close();

        return true;
    }

    std::string get(std::string value) {
        std::string key;
        get(value, key);
        return key;
    }

    // Delete key/value pair. Already encrypted so just delete file
    bool remove(std::string key)
    {

        Logger::debug("DBStore remove called");
        // Remove file in dbstore_directory
        // Filename = hash(key)[:10]

        std::string hashHex;
        Crypto::hashx(key, hashHex);

        std::string filename = hashHex.substr(0, 10) + ".db";

        std::filesystem::remove(dbstore_directory + "/" + filename);

        return true;
    }

    bool destroy()
    {
        // Delete dbstore_directory
        std::filesystem::remove_all(dbstore_directory);
        return true;
    }
}