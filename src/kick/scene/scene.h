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
#include <unordered_map>
#include <utility>
#include <functional>
#include <map>
#include "kick/scene/game_object.h"
#include "kick/scene/component.h"
#include "kick/core/event.h"
#include "kick/scene/scene_lights.h"

namespace kick {
    class Camera;
    class MeshRenderer;
    class Light;

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

        // helper function, which creates a gameobject and attaches a perspective camera to it
        Camera* createPerspectiveCamera();
        // helper function, which creates a gameobject and attaches an orthographic camera to it
        Camera* createOrthographicCamera();
        // helper function, which creates a gameobject and attaches a cube with diffuse renderer
        MeshRenderer* createCube();
        // helper function, which creates a gameobject and attaches a sphere with diffuse renderer
        MeshRenderer* createSphere();
        // helper function, which creates a gameobject and attaches a plane with diffuse renderer
        MeshRenderer* createPlane();
        // helper function, which creates a gameobject and attaches a point light
        Light* createPointLight();
        // helper function, which creates a gameobject and attaches a directional light
        Light* createDirectionalLight();
        // helper function, which creates a gameobject and attaches an ambient light
        Light* createAmbientLight(float intensity = 0.3f, glm::vec3 color = glm::vec3(1));


        friend class Engine;
        friend class GameObject;
    private:
        Scene(const char* name);
        Scene(const Scene& scene) = delete;
        void componentListener(Component* component, ComponentUpdateStatus status);
        void addLight(Light *light);
        std::vector<std::unique_ptr<GameObject>> gameObjects;
        std::map<GameObject*,EventListener<std::pair<Component*, ComponentUpdateStatus>>> componentListeners;
        std::vector<Camera*> cameras;
        std::vector<Component*> updatable;
        std::unordered_map<Light*, EventListener<Light*>> lights;
        SceneLights sceneLights;
        std::string name = "";

        void rebuildSceneLights();
    };
};


