//
//  component.cpp
//  KickCPP
//
//  Created by morten on 8/12/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/scene/engine_uniforms.h"
#include "kick/scene/component.h"
#include "kick/scene/game_object.h"

using namespace std;

namespace kick {
    
    Component::Component(GameObject* gameObject)
    : mGameObject(gameObject)
    {}
    
    Component::Component(Component&& component)
    : mGameObject(move(component.mGameObject))
    {
    }

    Component::~Component() {

    }

    Component& Component::operator=(const Component& other){
        if (this != &other){
            mGameObject = move(other.mGameObject);
        }
        return *this;
    }
    
    std::shared_ptr<Transform> Component::transform(){
        return mGameObject->transform();
    }

    GameObject *Component::gameObject() {
        return mGameObject;
    }

    bool Component::enabled() const {
        return mEnabled;
    }

    void Component::setEnabled(bool enabled) {
        if (Component::mEnabled != enabled){
            Component::mEnabled = enabled;

        }
    }
}