
#include <string>
#include <vector>
#include <common/logger.h>
#include <common/types.h>
#include <common/init.h>

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

    Logger::debug("Initialized successfully");
}