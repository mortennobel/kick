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
    
    Component::Component(GameObject* gameObject, bool renderable,bool updateable)
    :gameObject(gameObject), renderable(renderable),updateable(updateable)
    {}
    
    Component::Component(Component&& component)
    :gameObject(move(component.gameObject)), renderable(move(component.renderable)), updateable(move(component.updateable))
    {
    }

    Component::~Component() {

    }

    Component& Component::operator=(const Component& other){
        if (this != &other){
            gameObject = move(other.gameObject);
            renderable = other.renderable;
            updateable = other.updateable;
        }
        return *this;
    }
    
    bool Component::isRenderable(){
        return renderable;
    }
    
    bool Component::isUpdateable(){
        return updateable;
    }
    
    Transform* Component::getTransform(){
        return gameObject->getTransform();
    }

    GameObject *Component::getGameObject() {
        return gameObject;
    }
}