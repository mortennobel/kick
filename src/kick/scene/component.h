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
    class EngineUniforms;
    class Transform;
    
    enum class ComponentUpdateStatus {
        Created,
        Destroyed,
        Updated
    };
    
    class Component {
    public:
        Component(GameObject* gameObject, bool renderable, bool updateable);
        Component(Component&& component);
        Component& operator=(const Component& other);
        virtual ~Component();
        virtual void activated(){}
        virtual void deactivated(){}
        virtual void render(EngineUniforms *engineUniforms){}
        virtual void update(){}
        bool isRenderable();
        bool isUpdateable();
        Transform* getTransform();
        GameObject* getGameObject();
    protected:
        GameObject* gameObject;
    private:
        Component(const Component& component) = delete;
        bool renderable;
        bool updateable;
        //        int scriptPriority = 0;
        //        int renderOrder = 0;
    };
    
    typedef std::vector<Component*>::const_iterator ConstComponentIter;
    typedef std::vector<Component*>::iterator ComponentIter;
}