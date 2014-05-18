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
#include "kick/scene/mesh_renderer.h"
#include "kick/mesh/mesh_factory.h"
#include "kick/mesh/mesh.h"
#include "engine.h"
#include "light.h"
#include "glm/gtx/string_cast.hpp"

using namespace std;

namespace kick {
    
    Scene::Scene(const char* name)
    :name(name)
    {
    }
    
    Scene::~Scene(){
    }
    
    Scene::Scene(Scene&& scene)
    :gameObjects(move(scene.gameObjects)),
     cameras(move(scene.cameras)),
     name(move(scene.name))
    {}
    
    Scene& Scene::operator=(Scene&& other){
        if (this != &other){
            gameObjects = move(other.gameObjects);
            cameras = move(other.cameras);
            name = move(other.name);
        }
        return *this;
    }
    
    GameObject *Scene::createGameObject(const string &name){
        auto res = new GameObject(name, this);
        gameObjects.push_back(unique_ptr<GameObject>(res));
        return res;
    }

    void Scene::destroyGameObject(GameObject *gameObject){
        for (int i=0;i<gameObjects.size();i++){
            if (gameObjects[i].get() == gameObject){
                gameObjects.erase(gameObjects.begin()+i);
                return;
            }
        }
    }
    
    std::string Scene::getName() const{
        return name;
    }
    
    void Scene::setName(std::string name){
        this->name = name;
    }
    
    GameObjectIter Scene::begin() const {
        return gameObjects.begin();
    }
    
    GameObjectIter Scene::end() const {
        return gameObjects.end();
    }
        
    void Scene::update(){
        for (auto & component : updatable) {
            component->update();
        }
    }
    
    void Scene::render(EngineUniforms* engineUniforms){
        engineUniforms->sceneLights = &sceneLights;
        for (auto & camera : cameras) {
            engineUniforms->currentCamera = camera;
            camera->render(engineUniforms);
        }
    }
    
    void Scene::componentListener(Component* component, ComponentUpdateStatus status){
        componentEvents.notifyListeners({component, status});
        Camera *camera = dynamic_cast<Camera*>(component);
        Light *light = dynamic_cast<Light*>(component);
        if (status == ComponentUpdateStatus::Added){
            if (camera){
                cameras.push_back(camera);
            } else if (light){
                lights[light] = light->lightTypeChanged.createListener([&](Light* l){
                    rebuildSceneLights();
                }, 0);
                addLight(light);
            }
            if (component->isUpdateable()){
                updatable.push_back(component);
            }
        }
        if (status == ComponentUpdateStatus::Removed){
            if (camera){
                auto pos = find(cameras.begin(), cameras.end(), camera);
                if (pos != cameras.end()){
                    cameras.erase(pos);
                }
            } else if (light){
                // rebuild lights
                auto lightPos = lights.find(light);
                if (lightPos != lights.end()){
                    lights.erase(lightPos);
                    rebuildSceneLights();
                }
            }
            if (component->isUpdateable()){
                auto pos = find(updatable.begin(), updatable.end(), component);
                if (pos != updatable.end()){
                    updatable.erase(pos);
                }
            }
        }
    }

    void Scene::addLight(Light *light) {
        switch (light->getLightType()){
            case LightType::Ambient:
                this->sceneLights.ambientLight = light;
                cout << "Add ambient light " << glm::to_string(light->getColorIntensity()) << endl;
                break;
            case LightType::Directional:
                this->sceneLights.directionalLight = light;
                break;
            case LightType::Point:
                this->sceneLights.pointLights.push_back(light);
                break;
            case LightType::Uninitialized:
                cout << "" << endl;
                break;
        }
    }

    Camera* Scene::createPerspectiveCamera(){
        GameObject *cameraObject = createGameObject("PerspectiveCamera");
        Camera *cam = cameraObject->addComponent<Camera>();
        cam->setCameraType(CameraType::Perspective);
        cam->setNear(0.1f);
        cam->setFar(100);
        cam->setFieldOfView(60);
        cam->setClearColor(glm::vec4(0,0,0,1));
        return cam;
    }

    Camera *Scene::createOrthographicCamera() {
        GameObject *cameraObject = createGameObject("OrthographicCamera");
        Camera *cam = cameraObject->addComponent<Camera>();
        cam->setCameraType(CameraType::Orthographic);
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
        Mesh* mesh = Project::createAsset<Mesh>();
        mesh->setMeshData(MeshFactory::createCubeData());
        meshRenderer->setMesh(mesh);

        Shader * shader = Project::loadShader("assets/shaders/diffuse.shader");
        Material* mat = Project::createAsset<Material>();
        mat->setShader(shader);
        meshRenderer->setMaterial(mat);
        return meshRenderer;
    }

    MeshRenderer* Scene::createSphere(){
        GameObject *gameObject = createGameObject("Sphere");
        MeshRenderer *meshRenderer = gameObject->addComponent<MeshRenderer>();
        Mesh* mesh = Project::createAsset<Mesh>();
        mesh->setMeshData(MeshFactory::createUVSphereData());
        meshRenderer->setMesh(mesh);

        Shader * shader = Project::loadShader("assets/shaders/diffuse.shader");
        Material* mat = Project::createAsset<Material>();
        mat->setShader(shader);
        meshRenderer->setMaterial(mat);
        return meshRenderer;
    }

    MeshRenderer* Scene::createPlane(){
        GameObject *gameObject = createGameObject("Plane");
        MeshRenderer *meshRenderer = gameObject->addComponent<MeshRenderer>();
        Mesh* mesh = Project::createAsset<Mesh>();
        mesh->setMeshData(MeshFactory::createPlaneData());
        meshRenderer->setMesh(mesh);

        Shader * shader = Project::loadShader("assets/shaders/diffuse.shader");
        Material* mat = Project::createAsset<Material>();
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
        sceneLights.clear();
        for (auto & l : lights){
            addLight(l.first);
        }
    }
}