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
#include "kick/scene/engine_uniforms.h"
#include "kick/core/event.h"

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
        // deprecated
        virtual void activated() final {}
        virtual void deactivated(){}
        Transform* getTransform();
        GameObject* getGameObject();

        bool isEnabled() const;
        void setEnabled(bool enabled);
    protected:
        GameObject* gameObject;
    private:
        Component(const Component& component) = delete;
        bool enabled = true;
    };
    
    typedef std::vector<Component*>::const_iterator ConstComponentIter;
    typedef std::vector<Component*>::iterator ComponentIter;
}