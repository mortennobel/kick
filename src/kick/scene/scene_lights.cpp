//
//  scene_lights.cpp
//  KickCPP
//
//  Created by morten on 8/14/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/scene/scene_lights.h"
#include <iostream>
#include <algorithm>
#include "kick/scene/component.h"
#include "kick/scene/light.h"
#include "kick/scene/game_object.h"
#include "kick/scene/transform.h"
#include "glm/gtx/quaternion.hpp"

using namespace std;
using namespace glm;

namespace kick {
    
    void SceneLights::recomputeLight(mat4 viewMatrix){
        mat3 viewMatrixRotation = (mat3)viewMatrix;
        if (directionalLight){
            // compute light direction
            vec3 lightDirection{0,0,-1};
            auto transform = directionalLight->getGameObject()->getTransform();
            directionalLightWorld = rotate(transform->getRotation(), lightDirection);

            // transform to eye space
            vec3 directionalLightDirection = normalize(viewMatrixRotation * directionalLightWorld);
            directionalLightData[0] = directionalLightDirection;

            // compute half vector
            directionalLightData[2] = normalize(vec3(0,0,-1) + directionalLightDirection);

            directionalLightData[1] = directionalLight->getColorIntensity();
        }
        int i = 0;
        for (;i<std::min((size_t)KICK_MAX_POINT_LIGHTS, pointLights.size());i++){
            Light * light = pointLights[i];
            Transform * transform = light->getGameObject()->getTransform();

            // save eyespace position
            pointLightData[i][0] = viewMatrixRotation * transform->getPosition();
            pointLightData[i][1] = light->getColorIntensity();
            pointLightData[i][2] = light->getAttenuation();
        }
        // reset unused
        for (;i<KICK_MAX_POINT_LIGHTS;i++){
            pointLightData[i] = mat3(0);
        }
          /*  var index = 0,
                    i,
                    pointLight,
                    pointLightPosition;
            for (i = pointLights.length - 1; i >= 0; i--) {
                pointLight = pointLights[i];
                pointLightPosition = pointLight.transform.position;

                Mat4.multiplyVec3(pointLightDataVec3[index], viewMatrix, pointLightPosition);
                Vec3.copy(pointLightDataVec3[index + 1], pointLight.colorIntensity);
                Vec3.copy(pointLightDataVec3[index + 2], pointLight.attenuation);
                index += 3;
            }
                      */
    }

    void SceneLights::clear() {
        ambientLight = nullptr;
        directionalLight = nullptr;
        pointLights.clear();
        directionalLightWorld = vec3{0};
    }

}