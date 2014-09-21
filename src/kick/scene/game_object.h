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
#include "kick/scene/component.h"
#include "kick/scene/transform.h"
#include "kick/core/event.h"


namespace kick {
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
        template <typename C>
        C* getComponentInParent();
        template <typename C>
        std::vector<C*> getComponentsInParent();
        template <typename C>
        C* getComponentInChildren();
        template <typename C>
        std::vector<C*> getComponentsInChildren();
        bool destroyComponent(Component *component);
        ConstComponentIter begin() const;
        ConstComponentIter end() const;
        ComponentIter begin();
        ComponentIter end();
        Event<std::pair<Component*, ComponentUpdateStatus>> componentEvent;
        std::string getName() const;
        void setName(std::string str);

        Transform *getTransform(){ return transform; }
        
        Scene* getScene(){ return scene; }

        int getLayer() const;
        void setLayer(int layer);
        friend class Scene;

        int32_t getUniqueId();
    private:
        GameObject(const std::string &name, Scene *scene, int uniqueId);
        Scene *scene;
        int uniqueId;
        std::string name;
        int layer = 1;
        bool destroyed = false;
        std::vector<Component*> components;
        std::vector<Component*> newComponents;
        std::vector<std::function<void (Component*, ComponentUpdateStatus)>> componentListeners;
        Transform *transform;
    };
};

#include "game_object.inl"
