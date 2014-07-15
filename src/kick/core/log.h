//
// Created by morten on 15/07/14.
//

#pragma once

#include <string>

#define logInfo(x) (kick::Log::info(x, __func__, __FILE__, __LINE__))
#define logWarning(x) (kick::Log::warning(x, __func__, __FILE__, __LINE__))
#define logError(x) (kick::Log::error(x, __func__, __FILE__, __LINE__))

namespace kick {
    class Log {
    public:
        static void info(std::string message, std::string func, std::string file, int line);
        static void warning(std::string message, std::string func, std::string file, int line);
        static void error(std::string message, std::string func, std::string file, int line);
    };
};



