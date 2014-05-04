//
//  window_config.h
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 21/01/14.
//  Copyright (c) 2014 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once

#include <string>

namespace kick {
    struct WindowConfig{
        std::string name = "Kick";
        // bool fullscreen = false;
        unsigned int width = 800;
        unsigned int height = 600;
        // 0 means no depth buffer
        int depthBufferSize = 24;
        // bool stencilbuffer = false;
        
        static const WindowConfig plain;
    };
}