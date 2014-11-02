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
        C *addComponent(T... t);
        template <typename C>
        C* component();
        template <typename C>
        std::vector<C*> components();
        template <typename C>
        C*componentInParent();
        template <typename C>
        std::vector<C*> componentsInParent();
        template <typename C>
        C*componentInChildren();
        template <typename C>
        std::vector<C*> componentsInChildren();
        bool destroyComponent(Component *component);
        ConstComponentIter begin() const;
        ConstComponentIter end() const;
        ComponentIter begin();
        ComponentIter end();
        Event<std::pair<Component*, ComponentUpdateStatus>> componentEvent;
        std::string name() const;
        void setName(std::string str);

        Transform *transform(){ return mTransform; }
        
        Scene*scene(){ return mScene; }

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
        std::vector<Component*> mComponents;
        std::vector<Component*> newComponents;
        std::vector<std::function<void (Component*, ComponentUpdateStatus)>> componentListeners;
        Transform *mTransform;
    };
};

#include "game_object.inl"
