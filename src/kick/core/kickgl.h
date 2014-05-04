//
//  kickgl.h
//  KickCPP
//
//  Created by morten on 8/13/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once

#include <OpenGL/gl3.h>

#define BUFFER_OFFSET(x)  ((const void*) (x))

namespace kick {
    bool openglUsingVao();
}