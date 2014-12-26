//
// Created by morten on 15/07/14.
//

#pragma once

#include <string>
#include <functional>

#ifdef _WIN32
#define logInfo(x) (kick::Debug::info(x, __FUNCTION__, __FILE__, __LINE__))
#define logWarning(x) (kick::Debug::warning(x, __FUNCTION__, __FILE__, __LINE__))
#define logError(x) (kick::Debug::error(x, __FUNCTION__, __FILE__, __LINE__))
#else
#define logInfo(x) (kick::Debug::info(x, __func__, __FILE__, __LINE__))
#define logWarning(x) (kick::Debug::warning(x, __func__, __FILE__, __LINE__))
#define logError(x) (kick::Debug::error(x, __func__, __FILE__, __LINE__))
#endif 

namespace kick {
    class Debug {
    public:
        static std::function<void (std::string message, std::string func, std::string file, int line)> info;
        static std::function<void (std::string message, std::string func, std::string file, int line)> warning;
        static std::function<void (std::string message, std::string func, std::string file, int line)> error;

        /// Disable logging
        static void disable();
        /// Set logging to std out and err out streams
        static void reset();

        static void printStacktrace();

		static std::string workingDir();
    };
};



