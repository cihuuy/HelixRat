#pragma once

#include <string>
#include <common/config.h>

// Path: source/include/logger.h

namespace helixrat::Logger {
    bool init();
    void info(std::string message);
    void debug(std::string message);
    void warn(std::string message);
    void error(std::string message);
    void fatal(std::string message);
    void log(std::string message);
}