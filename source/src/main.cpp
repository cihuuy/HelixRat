
#include <string>
#include <vector>
#include <common/logger.h>
#include <common/types.h>
#include <common/init.h>
#include <common/db.h>

using namespace helixrat;

int main(int argc, char *argv[])
{
    strings_vector args(argv + 1, argv + argc);

    // Initialize logger
    bool init_status = helixrat::init(args);
    if (!init_status)
    {
        return FAIL_INIT_ERROR;
    }
    if (args[0] == "e")
    {
        DBStore::set("test", "Value set test dbstore");
        DBStore::set("testmasta2", ".Value set test dbstore");
        Logger::info("Value set");
    }
    else
    {
        std::string value;
        DBStore::get("test", value);

        Logger::info("Value: " + value);
    }
}