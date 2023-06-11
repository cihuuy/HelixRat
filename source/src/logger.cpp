#include <common/logger.h>
#include <iostream>

namespace helixrat::Logger
{
    bool init()
    {
        return true;
    }

    void log(std::string message)
    {
        if (LOG_LEVEL >= LOG_LEVEL_LOG)
            std::cout << message << std::endl;
    }

    void debug(std::string message)
    {
        if (LOG_LEVEL >= LOG_LEVEL_DEBUG)
            std::cout << "[DEBUG] " << message << std::endl;
    }

    void info(std::string message)
    {
        if (LOG_LEVEL >= LOG_LEVEL_INFO)
            std::cout << "[INFO] " << message << std::endl;
    }

    void warn(std::string message)
    {
        if (LOG_LEVEL >= LOG_LEVEL_WARN)
            std::cout << "[WARN] " << message << std::endl;
    }

    void error(std::string message)
    {
        if (LOG_LEVEL >= LOG_LEVEL_ERROR)
            std::cout << "[ERROR] " << message << std::endl;
    }

    void fatal(std::string message)
    {
        if (LOG_LEVEL >= LOG_LEVEL_FATAL)
            std::cout << "[FATAL] " << message << std::endl;
    }
}