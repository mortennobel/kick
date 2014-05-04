//
//  Scene.h
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 5/21/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <utility>
#include "game_object.h"
#include "component.h"
#include "kick/core/event.h"

namespace kick {
    class Camera;
    
    class Scene {
    public:
        Scene(Scene&& scene);
        Scene& operator=(Scene&& other);
        virtual ~Scene();
        GameObject *createGameObject(const std::string &name = "gameobject");
        void destroyGameObject(GameObject * gameObject);
        GameObjectIter begin() const;
        GameObjectIter end() const;
        std::string getName() const;
        void setName(std::string name);
        void update();
        void render(EngineUniforms* engineUniforms);
        Event<std::pair<Component*, ComponentUpdateStatus>> componentEvents;
        friend class Engine;
        friend class GameObject;
    private:
        Scene(const char* name);
        Scene(const Scene& scene) = delete;
        void componentListener(Component* component, ComponentUpdateStatus status);
        std::vector<std::unique_ptr<GameObject>> gameObjects;
        std::vector<Camera*> cameras;
        std::vector<Component*> updatable;
        std::string name = "";
    };
};


