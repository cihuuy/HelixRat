#include <common/db_store.h>
#include <common/logger.h>
#include <fstream>
#include <common/crypto.h>
#include <string.h>

#define DEFAULT_DBSTORE_FILE "dbstore.db"
#define DBSTORE_PASSWORD "password"

namespace helixrat::DBStore
{
    static Crypto::secret_t *dbstore_secret;
#define DBSTORE_VERSION 0x01
#define DBSTORE_MAXSIZE 4096
    struct db_store_header_t
    {
        // disguise as other file type
        char magic_mask[16];
        // The real magic
        char magic[4];
        uint8_t checksum;
        // Everything after this is encrypted
    };

    struct db_store_header_encrypted_t
    {
        uint16_t version;
        uint32_t entry_count;
        uint32_t max_size;
        uint32_t entry_offset;
        uint32_t checksum;
    };

    struct db_store_entry_t
    {
        uint8_t key_size;
        uint16_t value_size;
        uint16_t checksum;
        uint32_t key_offset;
        uint32_t value_offset;
    };
    
    uint32_t rotateRight(uint32_t x, int n)
    {
        uint32_t shifted = x >> n;

        uint32_t rot_bits = x << (32 - n);

        uint32_t combined = shifted | rot_bits;

        return combined;
    }

    uint32_t make_checksum(uint8_t *data, uint32_t length)
    {
        uint32_t checksum = 0x30e1997e;
        for (uint32_t i = 0; i < length; i++)
        {
            checksum ^= (uint32_t)data[i] << (i % 32);
        }

        for (uint32_t i = 0; i < length; i++)
        {
            checksum ^= (uint32_t)data[i] >> (rotateRight((uint32_t)data[i], i) % 32);
        }
        return checksum;
    }
    // Mask as a Sqlite3 database
    db_store_header_t init_header()
    {
        db_store_header_t header;
        uint8_t magic_bytes[] = {0x53, 0x51, 0x4c, 0x69, 0x74, 0x65, 0x20, 0x66, 0x6f, 0x72, 0x6d, 0x61, 0x74, 0x20, 0x33, 0x00};

        memcpy(header.magic_mask, magic_bytes, 16);
        memcpy(header.magic, "DBS1", 4);
        header.checksum = 0x00;
        uint32_t checksum = make_checksum((uint8_t *)&header, sizeof(header));
        header.checksum = checksum & 0xFF;
        return header;
    }

    db_store_header_encrypted_t make_header_encrypted(uint32_t entry_count)
    {
        db_store_header_encrypted_t header_encrypted;
        header_encrypted.version = DBSTORE_VERSION;
        header_encrypted.entry_count = entry_count;
        header_encrypted.max_size = DBSTORE_MAXSIZE;
        header_encrypted.entry_offset = sizeof(db_store_header_encrypted_t) + sizeof(db_store_header_t);
        header_encrypted.checksum = 0x00;
        uint32_t checksum = make_checksum((uint8_t *)&header_encrypted, sizeof(header_encrypted));
        header_encrypted.checksum = checksum;
        std::string ciphertext;
        // encrypt header
        Crypto::encrypt(std::string((char *)&header_encrypted, sizeof(db_store_header_encrypted_t)), *dbstore_secret, ciphertext);
        memcpy(&header_encrypted, ciphertext.c_str(), sizeof(header_encrypted));
        return header_encrypted;
    }

    bool init_dbstore(std::fstream &db_store_file)
    {
        Logger::debug("DBStore init_dbstore called");
        db_store_header_t header = init_header();
        db_store_header_encrypted_t header_encrypted = make_header_encrypted(0);
        db_store_file.write((char *)&header, sizeof(header));
        db_store_file.write((char *)&header_encrypted, sizeof(header_encrypted));
        return true;
    }

    bool verify_header(std::fstream &file)
    {
        Logger::debug("DBStore verify_header called");
        db_store_header_t header;
        file.seekg(0, std::ios::beg);
        file.read((char *)&header, sizeof(header));
        if (memcmp(header.magic, "DBS1", 4) != 0)
        {
            Logger::error("DBStore header magic mismatch");
            return false;
        }
        uint8_t tmp = header.checksum;
        header.checksum = 0x00;
        uint8_t checksum = (uint8_t)make_checksum((uint8_t *)&header, sizeof(header));
        if (checksum != tmp)
        {
            Logger::error("DBStore header checksum mismatch");
            return false;
        }
        return true;
    }

    error_t init(Crypto::secret_t &secret)
    {
        Logger::debug("DBStore init called");
        std::fstream db_store_file(DEFAULT_DBSTORE_FILE, std::ios_base::in | std::ios_base::out | std::ios_base::binary | std::ios_base::app);

        if (!db_store_file.is_open())
        {
            Logger::error("Failed to open dbstore file");
            return FILE_OPEN_ERROR;
        }
        dbstore_secret = &secret;

        // Check if file is empty
        db_store_file.seekg(0, std::ios::end);
        if (db_store_file.tellg() == 0)
        {
            Logger::debug("DBStore file is empty, initializing");
            // Initialize file
            if (init_dbstore(db_store_file))
            {
                Logger::debug("DBStore file initialized successfully");
                db_store_file.close();
                return SUCCESS;
            }
            else
            {
                Logger::error("Failed to initialize DBStore file");
                db_store_file.close();
                return FAIL_INIT_DBSTORE;
            }
        }

        // Check if file is valid
        if (!verify_header(db_store_file))
        {
            db_store_file.close();
            Logger::error("DBStore file is invalid");
            return FAIL_INIT_DBSTORE;
        }

        db_store_file.close();

        return SUCCESS;
    }
    error_t store(std::string key, std::string value)
    {
        Logger::debug("DBStore store called");
        return SUCCESS;
    }
    error_t get(std::string key, std::string &value)
    {
        Logger::debug("DBStore get called");
        return SUCCESS;
    }
    error_t remove(std::string key)
    {
        Logger::debug("DBStore remove called");
        return SUCCESS;
    }
    error_t destroy()
    {
        Logger::debug("DBStore destroy called");
        return SUCCESS;
    }
}