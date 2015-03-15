//
// Created by morten on 15/07/14.
//

#pragma once

#include <string>
#include <functional>
#include "glm/glm.hpp"

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
    class Camera;

    class Debug {
    public:
        static std::function<void (std::string message, std::string func, std::string file, int line)> info;
        static std::function<void (std::string message, std::string func, std::string file, int line)> warning;
        static std::function<void (std::string message, std::string func, std::string file, int line)> error;

        static void drawLine(glm::vec3 from, glm::vec3 to, float seconds, glm::vec4 color = glm::vec4(1.0), Camera* camera = nullptr);
        static void drawSphere(glm::vec3 center, float size, float seconds, glm::vec4 color = glm::vec4(1.0), Camera* camera = nullptr);
        static void drawBox(glm::vec3 center, glm::vec3 size, float seconds, glm::vec4 color = glm::vec4(1.0), Camera* camera = nullptr);

        /// Disable logging
        static void disable();
        /// Set logging to std out and err out streams
        static void reset();

        static void printStacktrace();

		static std::string workingDir();
    };
};



