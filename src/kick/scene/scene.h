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
#include "kick/scene/line_renderer.h"
#include "kick/scene/camera_orthographic.h"
#include "kick/2d/text.h"

namespace kick {
    class CameraPerspective;
    class CameraOrthographic;
    class MeshRenderer;
    class Light;
    class Updatable;
    class Text;
    class Sprite;
    class TextureAtlas;
    class Button;
    class Panel;

    class Scene {
    public:
        Scene(Scene&& scene);
        Scene& operator=(Scene&& other);
        virtual ~Scene();
        GameObject *createGameObject(const std::string &name = "gameobject");
        void destroyGameObject(GameObject * gameObject);
        GameObjectIter begin() const;
        GameObjectIter end() const;
        std::string name() const;
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
        CameraPerspective* createPerspectiveCamera(GameObject *go = nullptr);
        // Creates a gameobject and attaches an orthographic camera to it
        CameraOrthographic* createOrthographicCamera(GameObject *go = nullptr);
        // Creates a gameobject and attaches a line renderer with diffuse renderer
        LineRenderer* createLine(GameObject *go = nullptr, const std::vector<glm::vec3> &points = std::vector<glm::vec3>(), MeshType meshType = MeshType::Lines, const std::vector<GLushort> &indices = std::vector<GLushort>{});
        // Creates a gameobject and attaches a cube with diffuse renderer
        MeshRenderer* createCube(GameObject *go = nullptr);
        // Creates a gameobject and attaches a sphere with diffuse renderer
        MeshRenderer* createSphere(GameObject *go = nullptr);
        // Creates a gameobject and attaches a plane with diffuse renderer
        MeshRenderer* createPlane(GameObject *go = nullptr);
        // Creates a gameobject and attaches a point light
        Light* createPointLight(GameObject *go = nullptr);
        // Creates a gameobject and attaches a directional light
        Light* createDirectionalLight(GameObject *go = nullptr);
        // Creates a gameobject and attaches an ambient light
        Light* createAmbientLight(float intensity = 0.3f, glm::vec3 color = glm::vec3(1));
        // create a Panel
        Panel* createPanel(bool includeUICamera = true);

        friend class Engine;
        friend class GameObject;

        GameObject *gameObjectByUID(int32_t uid);

    private:
        Scene(const std::string & name);
        Scene(const Scene& scene) = delete;
        void componentListener(Component* component, ComponentUpdateStatus status);
        void addLight(Light *light);
        std::vector<std::unique_ptr<GameObject>> mGameObjects;
        std::map<GameObject*,EventListener<std::pair<Component*, ComponentUpdateStatus>>> mComponentListeners;
        std::vector<Camera*> mCameras;
        std::vector<Updatable *> mUpdatable;
        std::unordered_map<Light*, EventListener<Light*>> mLights;
        SceneLights mSceneLights;
        std::string mName = "";

        int32_t mUniqueIdGenerator = 0;

        void rebuildSceneLights();
    };
};


