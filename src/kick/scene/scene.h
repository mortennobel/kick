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
#include "kick/2d/label.h"

namespace kick {
    class CameraPerspective;
    class CameraOrthographic;
    class MeshRenderer;
    class Light;
    class Updatable;
    class Label;
    class Sprite;
    class TextureAtlas;
    class Button;
    class Canvas;

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
        Event<std::pair<std::shared_ptr<Component>, ComponentUpdateStatus>> componentEvents;

        template <typename T>
        std::vector<std::shared_ptr<T>> findComponents(){
            std::vector<std::shared_ptr<T>> res;
            for (auto & gameObject : *this){
                for (auto & comp : *gameObject){
                    auto component = std::dynamic_pointer_cast<T>(comp);
                    if (component){
                        res.push_back(component);
                    }
                }
            }
            return res;
        }

        template <typename T>
        std::shared_ptr<T> findComponent(){
            for (auto & gameObject : *this){
                for (auto & comp : *gameObject){
                    auto component = std::dynamic_pointer_cast<T>(comp);
                    if (component){
                        return component;
                    }
                }
            }
            return nullptr;
        }

        // Creates a gameobject and attaches a perspective camera to it
        std::shared_ptr<CameraPerspective> createPerspectiveCamera(GameObject *go = nullptr);
        // Creates a gameobject and attaches an orthographic camera to it
        std::shared_ptr<CameraOrthographic> createOrthographicCamera(GameObject *go = nullptr);
        // Creates a gameobject and attaches a line renderer with diffuse renderer
        std::shared_ptr<LineRenderer> createLine(GameObject *go = nullptr, const std::vector<glm::vec3> &points = std::vector<glm::vec3>(), MeshType meshType = MeshType::Lines, const std::vector<GLushort> &indices = std::vector<GLushort>{});
        // Creates a gameobject and attaches a cube with diffuse renderer
        std::shared_ptr<MeshRenderer> createCube(GameObject *go = nullptr, float length = 1);
        // Creates a gameobject and attaches a sphere with diffuse renderer
        std::shared_ptr<MeshRenderer> createSphere(GameObject *go = nullptr);
        // Creates a gameobject and attaches a plane with diffuse renderer
        std::shared_ptr<MeshRenderer> createPlane(GameObject *go = nullptr);
        // Creates a gameobject and attaches a point light
        std::shared_ptr<Light> createPointLight(GameObject *go = nullptr);
        // Creates a gameobject and attaches a directional light
        std::shared_ptr<Light> createDirectionalLight(GameObject *go = nullptr);
        // Creates a gameobject and attaches an ambient light
        std::shared_ptr<Light> createAmbientLight(float intensity = 0.3f, glm::vec3 color = glm::vec3(1));
        // create a Canvas
        std::shared_ptr<Canvas> createCanvas(bool includeUICamera = true);

        friend class Engine;
        friend class GameObject;

        GameObject *gameObjectByUID(int32_t uid);

        // Return the first camera component marked as main camera (or any camera if no camera marked).
        // Return nullptr if no camera component in scene
        std::shared_ptr<Camera> mainCamera();
    private:
        Scene(const std::string & name);
        Scene(const Scene& scene) = delete;
        void componentListener(std::shared_ptr<Component> component, ComponentUpdateStatus status);
        void addLight(std::shared_ptr<Light> light);
        std::vector<std::unique_ptr<GameObject>> mGameObjects;
        std::map<GameObject*,EventListener<std::pair<std::shared_ptr<Component>, ComponentUpdateStatus>>> mComponentListeners;
        std::vector<std::shared_ptr<Camera>> mCameras;
        std::vector<std::shared_ptr<Updatable>> mUpdatable;
        std::unordered_map<std::shared_ptr<Light>, EventListener<std::shared_ptr<Light>>> mLights;
        SceneLights mSceneLights;
        std::string mName = "";

        int32_t mUniqueIdGenerator = 0;

        void rebuildSceneLights();
    };
};


