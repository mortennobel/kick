//
//  texture_format.h
//  UnitTest_KickCPP
//
//  Created by morten on 07/04/14.
//
//

#pragma once

#include <iostream>
#include <functional>
#include <string>
#include "kick/core/kickgl.h"


namespace kick {
    enum class Mipmap {
        None = 0,
        Enabled = 1
    };
    
    struct ImageFormat {
        GLenum internalFormat = GL_RGB;
        GLenum format = GL_RGBA;
        GLenum type = GL_UNSIGNED_BYTE;
        int mipmapLevel = 0;
        Mipmap mipmap;
        bool isValid(std::function<void (std::string)> onError = [](std::string s){});
    };
}