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

#include <glm/glm.hpp>

namespace kick {
    class GameObject;
    
    enum class LightType{
        Uninitialized,
        Ambient,
        Directional,
        Point
    };

    enum class ShadowType{
        None,
        Hard,
        Soft
    };
    
    class Light : public Component {
    public:
        Light(GameObject *gameObject);
        void setColor(glm::vec3 color);
        glm::vec3 color();
        void setIntensity(float intensity);
        float intensity();
        glm::vec3 colorIntensity();
        void setLightType(LightType lightType);
        LightType lightType();
        glm::vec3 attenuation() const;
        void setAttenuation(glm::vec3 attenuation);
        Event<Light*> lightTypeChanged;
        ShadowType const &shadowType() const;
        void setShadowType(ShadowType const &shadowType);
    private:
        void updateIntensity();
        glm::vec3 mColorIntensity = glm::vec3(1,1,1);
        glm::vec3 mColor = glm::vec3(1,1,1);
        glm::vec3 mAttenuation = glm::vec3(1,0,0);
        float mIntensity = 1;
        LightType mLightType = LightType::Uninitialized;
        ShadowType mShadowType = ShadowType::None;
    };
}
