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
        template <typename C, typename... T>
        std::shared_ptr<C> addComponent(T... t);
        template <typename C>
        std::shared_ptr<C> component();
        template <typename C>
        std::vector<std::shared_ptr<C>> components();
        template <typename C>
        std::shared_ptr<C> componentInParent();
        template <typename C>
        std::vector<std::shared_ptr<C>> componentsInParent();
        template <typename C>
        std::shared_ptr<C> componentInChildren();
        template <typename C>
        std::vector<std::shared_ptr<C>> componentsInChildren();
        bool destroyComponent(std::shared_ptr<Component> component);
        ConstComponentIter begin() const;
        ConstComponentIter end() const;
        ComponentIter begin();
        ComponentIter end();
        Event<std::pair<std::shared_ptr<Component>, ComponentUpdateStatus>> componentEvent;
        std::string name() const;
        void setName(std::string str);

        std::shared_ptr<Transform> transform(){ return mTransform; }
        
        Scene* scene(){ return mScene; }

        int layer() const;
        void setLayer(int layer);
        friend class Scene;

        int32_t uniqueId();
    private:
        GameObject(const std::string &name, Scene *scene, int uniqueId);
        Scene *mScene;
        int mUniqueId;
        std::string mName;
        int mLayer = 1;
        bool mDestroyed = false;
        std::vector<std::shared_ptr<Component>> mComponents;
        std::vector<std::shared_ptr<Component>> newComponents;
        std::vector<std::function<void (std::shared_ptr<Component>, ComponentUpdateStatus)>> componentListeners;
        std::shared_ptr<Transform> mTransform;
    };
};

#include "game_object.inl"
