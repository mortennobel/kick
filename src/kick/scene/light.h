//
//  light.h
//  KickCPP
//
//  Created by morten on 8/15/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once

#include "component.h"
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
    private:
        void updateIntensity();
        glm::vec3 colorIntensity = glm::vec3(1,1,1);
        glm::vec3 color = glm::vec3(1,1,1);
        float intensity = 1;
        LightType lightType = LightType::Uninitialized;
    };
}
