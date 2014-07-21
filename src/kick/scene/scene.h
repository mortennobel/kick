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
#include "kick/scene/camera_perspective.h"
#include "kick/scene/camera_orthographic.h"
#include "kick/sprite/text.h"

namespace kick {
    class CameraPerspective;
    class CameraOrthographic;
    class MeshRenderer;
    class Light;
    class ComponentUpdateable;
    class Text;
    class Sprite;
    class TextureAtlas;
    class Button;

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

        template <typename T>
        std::vector<T*> findComponents(){
            std::vector<T*> res;
            for (auto & gameObject : *this){
                for (auto & comp : *gameObject){
                    T* component = dynamic_cast<T*>(comp);
                    if (component){
                        res.push_back(component);
                    }
                }
            }
            return res;
        }

        template <typename T>
        T* findComponent(){
            for (auto & gameObject : *this){
                for (auto & comp : *gameObject){
                    T* component = dynamic_cast<T*>(comp);
                    if (component){
                        return component;
                    }
                }
            }
            return nullptr;
        }

        // Creates a gameobject and attaches a perspective camera to it
        CameraPerspective* createPerspectiveCamera();
        // Creates a gameobject and attaches an orthographic camera to it
        CameraOrthographic* createOrthographicCamera();
        // Creates a gameobject and attaches a cube with diffuse renderer
        MeshRenderer* createCube();
        // Creates a gameobject and attaches a sphere with diffuse renderer
        MeshRenderer* createSphere();
        // Creates a gameobject and attaches a plane with diffuse renderer
        MeshRenderer* createPlane();
        // Creates a gameobject and attaches a point light
        Light* createPointLight();
        // Creates a gameobject and attaches a directional light
        Light* createDirectionalLight();
        // Creates a gameobject and attaches an ambient light
        Light* createAmbientLight(float intensity = 0.3f, glm::vec3 color = glm::vec3(1));
        //
        Button* createButton();
        // helper function, which creates a gameobject and attaches an sprite object
        Sprite* createSprite(std::shared_ptr<TextureAtlas> textureAtlas, std::string spriteName, glm::vec2 pos = glm::vec2{0});

        friend class Engine;
        friend class GameObject;

        Text *createText(std::string text);

    private:
        Scene(const char* name);
        Scene(const Scene& scene) = delete;
        void componentListener(Component* component, ComponentUpdateStatus status);
        void addLight(Light *light);
        std::vector<std::unique_ptr<GameObject>> gameObjects;
        std::map<GameObject*,EventListener<std::pair<Component*, ComponentUpdateStatus>>> componentListeners;
        std::vector<Camera*> cameras;
        std::vector<ComponentUpdateable*> updatable;
        std::unordered_map<Light*, EventListener<Light*>> lights;
        SceneLights sceneLights;
        std::string name = "";

        void rebuildSceneLights();
    };
};


