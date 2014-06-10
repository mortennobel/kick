//
//  component.h
//  KickCPP
//
//  Created by morten on 8/12/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once

#include <iostream>
#include <vector>
#include "engine_uniforms.h"

namespace kick {

    class GameObject;
    class Transform;
    
    enum class ComponentUpdateStatus {
        Created,
        Destroyed,
        Updated
    };
    
    class Component {
    public:
        Component(GameObject* gameObject);
        Component(Component&& component);
        Component& operator=(const Component& other);
        virtual ~Component();
        virtual void activated(){}
        virtual void deactivated(){}
        Transform* getTransform();
        GameObject* getGameObject();
    protected:
        GameObject* gameObject;
    private:
        Component(const Component& component) = delete;
    };
    
    typedef std::vector<Component*>::const_iterator ConstComponentIter;
    typedef std::vector<Component*>::iterator ComponentIter;
}