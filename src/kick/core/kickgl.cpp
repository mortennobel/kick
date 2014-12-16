//
//  kickgl.cpp
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 08/12/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/core/kickgl.h"

#include <iostream>

namespace kick {
    bool openglUsingVao(){
        return true;
    }

    const char *GLErrorString(GLenum errorCode) {
        static const struct {
            GLenum code;
            const char *string;
        } errors[]=
                {
                        /* GL */
                        {GL_NO_ERROR, "no error"},
                        {GL_INVALID_ENUM, "invalid enumerant"},
                        {GL_INVALID_VALUE, "invalid value"},
                        {GL_INVALID_OPERATION, "invalid operation"},
                        //        {GL_STACK_OVERFLOW, "stack overflow"},
                        //        {GL_STACK_UNDERFLOW, "stack underflow"},
                        {GL_OUT_OF_MEMORY, "out of memory"},
                        {0, ""}
                };
        int i;
        for (i=0; errors[i].code; i++)
        {
            if (errors[i].code == errorCode)
            {
                return errors[i].string;
            }
        }
        return nullptr;
    }

    int printOglError(const char *file, int line) {

        GLenum glErr;
        int    retCode = 0;

        glErr = glGetError();
        if (glErr != GL_NO_ERROR)
        {
            printf("glError in file %s @ line %d: %s\n",
                    file, line, GLErrorString(glErr));
            retCode = 1;
        }
        return retCode;
    }
}