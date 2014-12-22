//
//  kickgl.h
//  KickCPP
//
//  Created by morten on 8/13/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once

#if defined(_WIN32)
#define GLEW_STATIC 
#include <GL/glew.h>
#elif defined(KICK_CONTEXT_ES2)
#   include <GLES2/gl2.h>
#elif defined(EMSCRIPTEN)
#   ifndef KICK_CONTEXT_ES2
#       define KICK_CONTEXT_ES2 1
#   endif
#   include <GLES2/gl2.h>
#else
#   include <OpenGL/gl3.h>
#include <stdio.h>

#endif
#define BUFFER_OFFSET(x)  ((const void*) (x))

namespace kick {
    bool openglUsingVao();

#define printOpenGLError() printOglError(__FILE__, __LINE__)

    const char * GLErrorString(GLenum errorCode);

    int printOglError(const char *file, int line);
}