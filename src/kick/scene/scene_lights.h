//
//  scene_lights.h
//  KickCPP
//
//  Created by morten on 8/14/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once

#include <glm/glm.hpp>
#include <vector>

#ifndef KICK_MAX_POINT_LIGHTS
#define KICK_MAX_POINT_LIGHTS 3
#endif

namespace kick {
    class Light;
    
    struct SceneLights {
        Light *ambientLight;
        Light *directionalLight;
        std::vector<Light*> pointLights;
        void recomputeLight(glm::mat4 viewMatrix);
    };
}