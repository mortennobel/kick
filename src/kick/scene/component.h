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
    
    class Component : public std::enable_shared_from_this<Component> {
    public:
        Component(GameObject* gameObject);
        Component(Component&& component);
        Component& operator=(const Component& other);
        virtual ~Component();
        // deprecated
        virtual void activated() final {}
        virtual void deactivated(){}
        std::shared_ptr<Transform> transform();
        GameObject* gameObject();

        bool enabled() const;
        void setEnabled(bool enabled);
    protected:
        GameObject*mGameObject;
    private:
        Component(const Component& component) = delete;
        bool mEnabled = true;
    };
    
    typedef std::vector<std::shared_ptr<Component>>::const_iterator ConstComponentIter;
    typedef std::vector<std::shared_ptr<Component>>::iterator ComponentIter;
}