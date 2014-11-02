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
    :Component(gameObject)
    {
    }
    
    
    void Light::updateIntensity(){
        mColorIntensity = mColor * mIntensity;
    }
    
    void Light::setColor(glm::vec3 color) {
        this->mColor = color;
        updateIntensity();
    }
    
    glm::vec3 Light::color(){
        return mColor;
    }
    
    void Light::setIntensity(float intensity){
        this->mIntensity = intensity; updateIntensity();
    }
    
    float Light::intensity() {
        return mIntensity;
    }
    
    void Light::setLightType(LightType lightType) {
        assert(this->mLightType == LightType::Uninitialized);
        this->mLightType = lightType;
        lightTypeChanged.notifyListeners(this);
    };
    
    LightType Light::lightType(){
        return mLightType;
    }
    
    glm::vec3 Light::colorIntensity(){
        return mColorIntensity;
    }

    glm::vec3 Light::attenuation() const {
        return mAttenuation;
    }

    void Light::setAttenuation(glm::vec3 attenuation) {
        Light::mAttenuation = attenuation;
    }

    ShadowType const &Light::shadowType() const {
        return mShadowType;
    }

    void Light::setShadowType(ShadowType const &shadowType) {
        Light::mShadowType = shadowType;
    }


}