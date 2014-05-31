//
//  texture_config.h
//  UnitTest_KickCPP
//
//  Created by morten on 07/04/14.
//
//

#pragma once
#include <iostream>
#include "kick/core/kickgl.h"

namespace kick {
    enum class TextureWrap {
        ClampToEdge = GL_CLAMP_TO_EDGE,
        ClampToBorder = GL_CLAMP_TO_BORDER,
        MirroedRepeat = GL_MIRRORED_REPEAT,
        Repeat = GL_REPEAT
    };
    
    enum class TextureFilter {
        Nearest = GL_NEAREST,
        Linear = GL_LINEAR,
        NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
        LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
        NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
        LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR
    };
    
    struct TextureSampler {
        TextureWrap wrapS = TextureWrap::Repeat;
        TextureWrap wrapT = TextureWrap::Repeat;
        TextureWrap wrapW = TextureWrap::Repeat;
        TextureFilter filterMin = TextureFilter::Nearest;
        TextureFilter filterMag = TextureFilter::Nearest;
    };
}