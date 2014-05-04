//
//  engine_uniforms.h
//  KickCPP
//
//  Created by morten on 8/14/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once

#include "glm/glm.hpp"
#include "kick/scene/scene_lights.h"
#include "kick/core/event_listener.h"

namespace kick {
    class Camera;
    class Transform;
    
    struct EngineUniforms {
        SyncValue<glm::ivec2> viewportDimension;
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;
        glm::mat4 viewProjectionMatrix;
        glm::mat4 lightMatrix;
        Camera *currentCamera;
        Transform* currentCameraTransform;
        SceneLights sceneLights;
    };
}