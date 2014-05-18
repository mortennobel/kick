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
        Light *ambientLight = nullptr;
        Light *directionalLight = nullptr;
        std::vector<Light*> pointLights;

        glm::mat3 directionalLightData; // matrix with the columns lightDirection,colorIntensity,halfVector
        glm::mat3 pointLightData[KICK_MAX_POINT_LIGHTS];
        glm::vec3 directionalLightWorld;
        void recomputeLight(glm::mat4 viewMatrix);

        void clear();
    };
}