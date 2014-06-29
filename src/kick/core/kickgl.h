//
//  kickgl.h
//  KickCPP
//
//  Created by morten on 8/13/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once

#if defined(KICK_CONTEXT_ES2)
#include <GLES2/gl2.h>
#elif defined(EMSCRIPTEN)
#   include <GLES2/gl2.h>
#else
#   include <OpenGL/gl3.h>
#endif
#define BUFFER_OFFSET(x)  ((const void*) (x))

namespace kick {
    bool openglUsingVao();
}