//
//  GameObject.h
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 5/21/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include "component.h"

namespace kick {
    class Transform;
    class Scene;
    
    class GameObject {
    public:
        GameObject(const GameObject& other);
        GameObject(GameObject&& other);
        GameObject& operator=(GameObject&& other);
        ~GameObject();
        template <typename C>
        C *addComponent();
        template <typename C>
        C* getComponent();
        template <typename C>
        std::vector<C*> getComponents();
        bool removeComponent(Component* component);
        ConstComponentIter begin() const;
        ConstComponentIter end() const;
        ComponentIter begin();
        ComponentIter end();
        void addComponentListener(std::function<void (Component*, ComponentUpdateStatus)> &f);
        void removeComponentListener(std::function<void (Component*, ComponentUpdateStatus)> &f);
        std::string getName() const;
        void setName(std::string str);
        
        Transform *getTransform(){ return transform; }
        
        Scene* getScene(){ return scene; }
        
        friend class Scene;
    private:
        void fireComponentEvent(Component* component, ComponentUpdateStatus status);
        GameObject(const std::string &name, Scene *scene);
        Scene *scene;
        std::string name;
        bool destroyed = false;
        std::vector<Component*> components;
        std::vector<std::function<void (Component*, ComponentUpdateStatus)>> componentListeners;
        Transform *transform;
    };
};

#include "game_object.inl"
