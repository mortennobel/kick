//
//  light.cpp
//  KickCPP
//
//  Created by morten on 8/15/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/scene/light.h"
#include <cassert>

namespace kick {
    Light::Light(GameObject *gameObject)
    :Component(gameObject, false, false)
    {
    }
    
    
    void Light::updateIntensity(){
        colorIntensity = color * intensity;
    }
    
    void Light::setColor(glm::vec3 color) {
        this->color = color;
        updateIntensity();
    }
    
    glm::vec3 Light::getColor(){
        return color;
    }
    
    void Light::setIntensity(float intensity){
        this->intensity = intensity; updateIntensity();
    }
    
    float Light::getIntensity() {
        return intensity;
    }
    
    void Light::setLightType(LightType lightType) {
        assert(this->lightType == LightType::Uninitialized);
        this->lightType = lightType;
    };
    
    LightType Light::getLightType(){
        return lightType;
    }
    
    glm::vec3 Light::getColorIntensity(){
        return colorIntensity;
    }
}