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
    
    void Context::setMouseInput(MouseInput* mouseInput) {
        this->mouseInput = mouseInput;
    }
    
    void Context::setKeyInput(KeyInput* keyInput){
        this->keyInput = keyInput;
    }
}