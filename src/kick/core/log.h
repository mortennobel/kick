//
// Created by morten on 15/07/14.
//

#pragma once

#include <string>
#include <functional>

#define logInfo(x) (kick::Log::info(x, __func__, __FILE__, __LINE__))
#define logWarning(x) (kick::Log::warning(x, __func__, __FILE__, __LINE__))
#define logError(x) (kick::Log::error(x, __func__, __FILE__, __LINE__))

namespace kick {
    class Log {
    public:
        static std::function<void (std::string message, std::string func, std::string file, int line)> info;
        static std::function<void (std::string message, std::string func, std::string file, int line)> warning;
        static std::function<void (std::string message, std::string func, std::string file, int line)> error;

        /// Disable logging
        static void disable();
        /// Set logging to std out and err out streams
        static void reset();

        static void printStacktrace();
    };
};



