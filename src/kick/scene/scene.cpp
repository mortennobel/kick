//
//  Scene.cpp
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 5/21/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/scene/scene.h"
#include <iostream>
#include <algorithm>
#include "rapidjson/rapidjson.h"
#include "rapidjson/writer.h"
#include "kick/scene/engine_uniforms.h"
#include "kick/scene/camera.h"
#include "updatable.h"
#include "kick/scene/camera_orthographic.h"
#include "kick/scene/camera_perspective.h"
#include "kick/scene/mesh_renderer.h"
#include "kick/mesh/mesh_factory.h"
#include "kick/mesh/mesh.h"
#include "kick/core/engine.h"
#include "kick/scene/light.h"
#include "kick/2d/sprite.h"
#include "glm/gtx/string_cast.hpp"
#include "kick/texture/texture_atlas.h"
#include "kick/2d/button.h"
#include "kick/2d/canvas.h"

using namespace std;

namespace kick {
    
    Scene::Scene(const std::string & name)
    : mName(name)
    {
    }
    
    Scene::~Scene(){
    }
    
    Scene::Scene(Scene&& scene)
    : mGameObjects(move(scene.mGameObjects)),
     mCameras(move(scene.mCameras)),
     mName(move(scene.mName))
    {}
    
    Scene& Scene::operator=(Scene&& other){
        if (this != &other){
            mGameObjects = move(other.mGameObjects);
            mCameras = move(other.mCameras);
            mName = move(other.mName);
        }
        return *this;
    }
    
    GameObject *Scene::createGameObject(const string &name){
        auto res = new GameObject(name, this, ++mUniqueIdGenerator);
        EventListener<std::pair<std::shared_ptr<Component>, ComponentUpdateStatus>> eventListener = res->componentEvent.createListener([&](std::pair<std::shared_ptr<Component>, ComponentUpdateStatus> e){
            componentListener(e.first, e.second);
        });

        mGameObjects.push_back(unique_ptr<GameObject>(res));
        mComponentListeners[res] = move(eventListener);
        return res;
    }

    bool Scene::destroyGameObject(GameObject *gameObject){
        for (int i=0;i< mGameObjects.size();i++){
            if (mGameObjects[i].get() == gameObject){
                //explicit destroy (since gameObject may be referenced from other places)
                for (auto c : gameObject->components<Component>()){
                    gameObject->destroyComponent(c);
                }
                mComponentListeners.erase(gameObject);
                mGameObjects.erase(mGameObjects.begin()+i);
                return true;
            }
        }
        return false;
    }
    
    std::string Scene::name() const{
        return mName;
    }
    
    void Scene::setName(std::string name){
        this->mName = name;
    }
    
    GameObjectIter Scene::begin() const {
        return mGameObjects.begin();
    }
    
    GameObjectIter Scene::end() const {
        return mGameObjects.end();
    }
        
    void Scene::update() {
        for (auto & component : mUpdatable) {
            component->update();
        }
    }
    
    void Scene::render(EngineUniforms* engineUniforms){
        engineUniforms->sceneLights = &mSceneLights;
        std::sort(mCameras.begin(), mCameras.end(), [](std::shared_ptr<Camera> c1, std::shared_ptr<Camera> c2){
            return c1->index() < c2->index();
        });
        for (auto & camera : mCameras) {
            if (camera->enabled()){
                engineUniforms->currentCamera = camera;
                camera->render(engineUniforms);
            }
        }
    }
    
    void Scene::componentListener(std::shared_ptr<Component> component, ComponentUpdateStatus status){
        componentEvents.notifyListeners({component, status});
        auto camera = std::dynamic_pointer_cast<Camera>(component);
        auto light = std::dynamic_pointer_cast<Light>(component);
        if (status == ComponentUpdateStatus::Created){
            if (camera){
                mCameras.push_back(camera);
            } else if (light){
                mLights[light] = light->lightTypeChanged.createListener([&](std::shared_ptr<Light> l){
                    rebuildSceneLights();
                }, 0);
                addLight(light);
            }
            auto updateable = std::dynamic_pointer_cast<Updatable>(component);
            if (updateable){
                mUpdatable.push_back(updateable);
            }
        }
        if (status == ComponentUpdateStatus::Destroyed){
            if (camera){
                auto pos = find(mCameras.begin(), mCameras.end(), camera);
                if (pos != mCameras.end()){
                    mCameras.erase(pos);
                }
            } else if (light){
                // rebuild lights
                auto lightPos = mLights.find(light);
                if (lightPos != mLights.end()){
                    mLights.erase(lightPos);
                    rebuildSceneLights();
                }
            }
            auto updateable = std::dynamic_pointer_cast<Updatable>(component);
            if (updateable){
                auto pos = find(mUpdatable.begin(), mUpdatable.end(), updateable);
                if (pos != mUpdatable.end()){
                    mUpdatable.erase(pos);
                }
            }
        }
    }

    void Scene::addLight(std::shared_ptr<Light> light) {
        switch (light->lightType()){
            case LightType::Ambient:
                this->mSceneLights.ambientLight = light;
                break;
            case LightType::Directional:
                this->mSceneLights.directionalLight = light;
                break;
            case LightType::Point:
                this->mSceneLights.pointLights.push_back(light);
                break;
            case LightType::Spot:
                // todo implement
                break;
            case LightType::Uninitialized:
                break;
        }
    }

    std::shared_ptr<CameraPerspective> Scene::createPerspectiveCamera(GameObject *cameraObject){
        if (!cameraObject ){
            cameraObject = createGameObject("PerspectiveCamera");
        }
        auto cam = cameraObject->addComponent<CameraPerspective>();
        cam->setNear(0.1f);
        cam->setFar(100);
        cam->setFieldOfViewY(glm::radians(60.0f));
        cam->setClearColor(glm::vec4(0,0,0,1));
        return cam;
    }

    std::shared_ptr<CameraOrthographic> Scene::createOrthographicCamera(GameObject *cameraObject) {
        if (!cameraObject ) {
            cameraObject = createGameObject("OrthographicCamera");
        }
        auto cam = cameraObject->addComponent<CameraOrthographic>();
        glm::ivec2 dim = Engine::context()->getContextSurfaceDim();
        cam->setNear(-10);
        cam->setFar(10);
        cam->setLeft(-dim.x/2);
        cam->setRight(dim.x/2);
        cam->setBottom(-dim.y/2);
        cam->setTop(dim.y/2);
        cam->setClearColor(glm::vec4(0,0,0,1));
        return cam;
    }

    std::shared_ptr<MeshRenderer> Scene::createCube(GameObject *gameObject, float length){
        if (!gameObject) {
            gameObject = createGameObject("Cube");
        }
        auto meshRenderer = gameObject->addComponent<MeshRenderer>();
        auto mesh = make_shared<Mesh>();
        mesh->setMeshData(MeshFactory::createCubeData(length*0.5f));
        meshRenderer->setMesh(mesh);

        Material* mat = Project::createMaterial("assets/shaders/diffuse.shader");
        meshRenderer->setMaterial(mat);
        return meshRenderer;
    }

    std::shared_ptr<MeshRenderer> Scene::createSphere(GameObject *gameObject){
        if (!gameObject ) {
            gameObject = createGameObject("Sphere");
        }
        auto meshRenderer = gameObject->addComponent<MeshRenderer>();
        auto mesh = make_shared<Mesh>();
        mesh->setMeshData(MeshFactory::createUVSphereData());
        meshRenderer->setMesh(mesh);

        Material* mat = Project::createMaterial("assets/shaders/diffuse.shader");
        meshRenderer->setMaterial(mat);
        return meshRenderer;
    }


    std::shared_ptr<LineRenderer> Scene::createLine(GameObject *gameObject, const vector<glm::vec3> &points, kick::MeshType meshType, const std::vector<GLushort> &indices) {
        if (!gameObject) {
            gameObject = createGameObject("Plane");
        }
        auto lineRenderer = gameObject->addComponent<LineRenderer>();
        if (points.size()){
            lineRenderer->setPoints(points, meshType, indices);
        }
        return lineRenderer;
    }

    std::shared_ptr<MeshRenderer> Scene::createPlane(GameObject *gameObject){
        if (!gameObject) {
            gameObject = createGameObject("Plane");
        }
        auto meshRenderer = gameObject->addComponent<MeshRenderer>();
        auto mesh = make_shared<Mesh>();
        mesh->setMeshData(MeshFactory::createPlaneData());
        meshRenderer->setMesh(mesh);

        Material* mat = Project::createMaterial("assets/shaders/diffuse.shader");
        meshRenderer->setMaterial(mat);
        return meshRenderer;
    }

    std::shared_ptr<Light> Scene::createPointLight(GameObject *gameObject){
        if (!gameObject) {
            gameObject = createGameObject("PointLight");
        }
        auto light = gameObject->addComponent<Light>();
        light->setLightType(LightType::Point);
        return light;
    }

    std::shared_ptr<Light> Scene::createDirectionalLight(GameObject *gameObject){
        if (!gameObject){
            gameObject = createGameObject("DirectionalLight");
        }
        auto light = gameObject->addComponent<Light>();
        light->setLightType(LightType::Directional);
        return light;
    }

    // helper function, which creates a gameobject and attaches a ambient light
    std::shared_ptr<Light> Scene::createAmbientLight(float intensity, glm::vec3 color){
        GameObject *gameObject = createGameObject("AmbientLight");
        auto light = gameObject->addComponent<Light>();
        light->setLightType(LightType::Ambient);
        light->setIntensity(intensity);
        light->setColor(color);
        return light;
    }

    void Scene::rebuildSceneLights() {
        mSceneLights.clear();
        for (auto & l : mLights){
            addLight(l.first);
        }
    }

    std::shared_ptr<Canvas> Scene::createCanvas(bool includeUICamera) {
        GameObject *gameObject = createGameObject("Canvas");
        auto canvas = gameObject->addComponent<Canvas>();
        if (includeUICamera){
            auto camera = createOrthographicCamera(gameObject);
            camera->setMain(false);
            camera->setIndex(1);
            camera->setCullingMask(256);//0b100000000
            camera->setClearColorBuffer(false);
            canvas->setCamera(camera);
        }
        return canvas;
    }

    GameObject *Scene::gameObjectByUID(int32_t uid) {
        for (auto & gameObject : *this) {
            if (gameObject->uniqueId() == uid) {
                return gameObject.get();
            }
        }
        return nullptr;
    }

    std::shared_ptr<Camera> Scene::mainCamera() {
        for (auto c : mCameras) {
            if (c->main()) {
                return c;
            }
        }
        if (!mCameras.empty()) {
            return mCameras[0];
        }
        return std::shared_ptr<Camera>();
    }
}