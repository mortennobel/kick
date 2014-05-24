//
//  light.h
//  KickCPP
//
//  Created by morten on 8/15/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once

#include "kick/scene/component.h"
#include "kick/core/event.h"
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

namespace kick {
    class GameObject;
    
    enum class LightType{
        Uninitialized,
        Ambient,
        Directional,
        Point
    };
    
    class Light : public Component {
    public:
        Light(GameObject *gameObject);
        void setColor(glm::vec3 color);
        glm::vec3 getColor();
        void setIntensity(float intensity);
        float getIntensity();
        glm::vec3 getColorIntensity();
        void setLightType(LightType lightType);
        LightType getLightType();
        glm::vec3 getAttenuation() const;
        void setAttenuation(glm::vec3 attenuation);
        Event<Light*> lightTypeChanged;
    private:
        void updateIntensity();
        glm::vec3 colorIntensity = glm::vec3(1,1,1);
        glm::vec3 color = glm::vec3(1,1,1);
        glm::vec3 attenuation = glm::vec3(1,0,0);
        float intensity = 1;
        LightType lightType = LightType::Uninitialized;
    };
}
