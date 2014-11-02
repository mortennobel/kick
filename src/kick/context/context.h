//
//  PlatformBinding.h
//  KickCPP
//
//  Created by morten on 8/10/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once

#include <iostream>
#include <functional>
#include <string>

#include "glm/glm.hpp"
#include "kick/core/event.h"
#include "kick/core/touch_input.h"
#include "kick/context/window_config.h"

namespace kick {
    class MouseInput;
    class KeyInput;
    
    class Context {
    public:
        Context();
        virtual ~Context(){}
        virtual bool init(int &argc, char **argv) = 0;
        virtual bool showWindow(const WindowConfig& config) = 0;
        virtual void swapBuffer() = 0;
        virtual void mainLoop() = 0;

        virtual bool isFullscreen() = 0;
        virtual void setFullscreen(bool fullscreen) = 0;
        virtual void setWindowTitle(std::string title) = 0;
        virtual std::string getWindowTitle() = 0;
        /**
         * Return the resource base path
         */
        virtual std::string getBasePath();
        glm::ivec2 getContextSurfaceDim();
        Event<glm::ivec2> contextSurfaceSize;
        std::string getGLSLPrefix();

    protected:
        glm::ivec2 contextSurfaceDim = glm::ivec2{512, 512};
    };
};