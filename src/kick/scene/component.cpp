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
    :gameObject(gameObject)
    {}
    
    Component::Component(Component&& component)
    :gameObject(move(component.gameObject))
    {
    }

    Component::~Component() {

    }

    Component& Component::operator=(const Component& other){
        if (this != &other){
            gameObject = move(other.gameObject);
        }
        return *this;
    }
    
    Transform* Component::getTransform(){
        return gameObject->getTransform();
    }

    GameObject *Component::getGameObject() {
        return gameObject;
    }

    bool Component::isEnabled() const {
        return enabled;
    }

    void Component::setEnabled(bool enabled) {
        if (Component::enabled != enabled){
            Component::enabled = enabled;

        }
    }
}