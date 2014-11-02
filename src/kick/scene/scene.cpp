//
//  Scene.cpp
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 5/21/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/scene/scene.h"
#include <iostream>
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
#include "kick/2d/panel.h"

using namespace std;

namespace kick {
    
    Scene::Scene(const std::string & name)
    : mName{name}
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
        EventListener<std::pair<Component*, ComponentUpdateStatus>> eventListener = res->componentEvent.createListener([&](std::pair<Component*, ComponentUpdateStatus> e){
            componentListener(e.first, e.second);
        });

        mGameObjects.push_back(unique_ptr<GameObject>(res));
        mComponentListeners[res] = move(eventListener);
        return res;
    }

    void Scene::destroyGameObject(GameObject *gameObject){
        for (int i=0;i< mGameObjects.size();i++){
            if (mGameObjects[i].get() == gameObject){
                mComponentListeners.erase(gameObject);
                mGameObjects.erase(mGameObjects.begin()+i);
                return;
            }
        }
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
        
    void Scene::update(){
        for (auto & component : mUpdatable) {
            component->update();
        }
    }
    
    void Scene::render(EngineUniforms* engineUniforms){
        engineUniforms->sceneLights = &mSceneLights;
        for (auto & camera : mCameras) {
            engineUniforms->currentCamera = camera;
            camera->render(engineUniforms);
        }
    }
    
    void Scene::componentListener(Component* component, ComponentUpdateStatus status){
        componentEvents.notifyListeners({component, status});
        Camera *camera = dynamic_cast<Camera*>(component);
        Light *light = dynamic_cast<Light*>(component);
        if (status == ComponentUpdateStatus::Created){
            if (camera){
                mCameras.push_back(camera);
            } else if (light){
                mLights[light] = light->lightTypeChanged.createListener([&](Light* l){
                    rebuildSceneLights();
                }, 0);
                addLight(light);
            }
            auto updateable = dynamic_cast<Updatable *>(component);
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
            auto updateable = dynamic_cast<Updatable *>(component);
            if (updateable){
                auto pos = find(mUpdatable.begin(), mUpdatable.end(), updateable);
                if (pos != mUpdatable.end()){
                    mUpdatable.erase(pos);
                }
            }
        }
    }

    void Scene::addLight(Light *light) {
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
            case LightType::Uninitialized:
                break;
        }
    }

    CameraPerspective* Scene::createPerspectiveCamera(){
        GameObject *cameraObject = createGameObject("PerspectiveCamera");
        CameraPerspective *cam = cameraObject->addComponent<CameraPerspective>();
        cam->setNear(0.1f);
        cam->setFar(100);
        cam->setFieldOfView(glm::radians(60.0f));
        cam->setClearColor(glm::vec4(0,0,0,1));
        return cam;
    }

    CameraOrthographic *Scene::createOrthographicCamera() {
        GameObject *cameraObject = createGameObject("OrthographicCamera");
        CameraOrthographic *cam = cameraObject->addComponent<CameraOrthographic>();
        cam->setNear(-10);
        cam->setFar(10);
        cam->setLeft(-10);
        cam->setRight(10);
        cam->setBottom(-10);
        cam->setTop(10);
        cam->setClearColor(glm::vec4(0,0,0,1));
        return cam;
    }

    MeshRenderer* Scene::createCube(){
        GameObject *gameObject = createGameObject("Cube");
        MeshRenderer *meshRenderer = gameObject->addComponent<MeshRenderer>();
        Mesh* mesh = new Mesh();
        mesh->setMeshData(MeshFactory::createCubeData());
        meshRenderer->setMesh(mesh);

        auto shader = Project::loadShader("assets/shaders/diffuse.shader");
        Material* mat = new Material();
        mat->setShader(shader);
        meshRenderer->setMaterial(mat);
        return meshRenderer;
    }

    MeshRenderer* Scene::createSphere(){
        GameObject *gameObject = createGameObject("Sphere");
        MeshRenderer *meshRenderer = gameObject->addComponent<MeshRenderer>();
        Mesh* mesh = new Mesh();
        mesh->setMeshData(MeshFactory::createUVSphereData());
        meshRenderer->setMesh(mesh);

        auto shader = Project::loadShader("assets/shaders/diffuse.shader");
        Material* mat = new Material();
        mat->setShader(shader);
        meshRenderer->setMaterial(mat);
        return meshRenderer;
    }

    MeshRenderer* Scene::createPlane(){
        GameObject *gameObject = createGameObject("Plane");
        MeshRenderer *meshRenderer = gameObject->addComponent<MeshRenderer>();
        Mesh* mesh = new Mesh();
        mesh->setMeshData(MeshFactory::createPlaneData());
        meshRenderer->setMesh(mesh);

        auto shader = Project::loadShader("assets/shaders/diffuse.shader");
        Material* mat = new Material();
        mat->setShader(shader);
        meshRenderer->setMaterial(mat);
        return meshRenderer;
    }

    Light* Scene::createPointLight(){
        GameObject *gameObject = createGameObject("PointLight");
        Light* light = gameObject->addComponent<Light>();
        light->setLightType(LightType::Point);
        return light;
    }

    Light* Scene::createDirectionalLight(){
        GameObject *gameObject = createGameObject("DirectionalLight");
        Light* light = gameObject->addComponent<Light>();
        light->setLightType(LightType::Directional);
        return light;
    }

    // helper function, which creates a gameobject and attaches a ambient light
    Light* Scene::createAmbientLight(float intensity, glm::vec3 color){
        GameObject *gameObject = createGameObject("AmbientLight");
        Light* light = gameObject->addComponent<Light>();
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

    Panel *Scene::createPanel(bool includeUICamera) {
        GameObject *gameObject = createGameObject("Panel");
        Panel* panel = gameObject->addComponent<Panel>();
        if (includeUICamera){
            Camera* camera = gameObject->addComponent<Camera>();
            camera->setCullingMask(256);
            panel->setCamera(camera);
        }
        return panel;
    }

    GameObject *Scene::gameObjectByUID(int32_t uid) {
        for (auto & gameObject : *this){
            if (gameObject->uniqueId() == uid){
                return gameObject.get();
            }
        }
        return nullptr;
    }
}