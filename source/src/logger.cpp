#include <common/logger.h>
#include <iostream>

namespace helixrat::Logger {
    bool init()
    {
        return true;
    }

    void log(std::string message)
    {
        std::cout << message << std::endl;
    }

    void debug(std::string message)
    {
        std::cout << "[DEBUG] " << message << std::endl;
    }

    void info(std::string message)
    {
        std::cout << "[INFO] " << message << std::endl;
    }

    void warn(std::string message)
    {
        std::cout << "[WARN] " << message << std::endl;
    }

    void error(std::string message)
    {
        std::cout << "[ERROR] " << message << std::endl;
    }

    void fatal(std::string message)
    {
        std::cout << "[FATAL] " << message << std::endl;
    }
}