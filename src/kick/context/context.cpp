//
//  context.cpp
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 27/12/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/context/context.h"
#include "kick/core/mouse_input.h"

using namespace std;
using namespace glm;

namespace kick {

    Context::Context()
    {}

    glm::ivec2 Context::getContextSurfaceDim(){
        return contextSurfaceDim;
    }
    
    string Context::getGLSLPrefix(){
        return "#version 150";
    }
    
    std::string Context::getBasePath(){
        return "./";
    }

    Texture2DData Context::getScreenAsTextureData() {
        // since we need to read from front buffer
        swapBuffer();

        ivec2 dim = getContextSurfaceDim();
        vector<char> data(dim.x*dim.y*4);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glReadPixels(0, 0, dim.x, dim.y, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
        swapBuffer();
        return Texture2DData(data, dim.x, dim.y);
    }
}