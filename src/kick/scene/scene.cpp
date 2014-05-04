//
//  Scene.cpp
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 5/21/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/scene/scene.h"
#include "kick/scene/engine_uniforms.h"
#include "kick/scene/camera.h"
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
        for (auto & camera : cameras) {
            engineUniforms->currentCamera = camera;
            camera->render(engineUniforms);
        }
    }
    
    void Scene::componentListener(Component* component, ComponentUpdateStatus status){
        componentEvents.notifyListeners({component, status});
        Camera *camera = dynamic_cast<Camera*>(component);
        if (status == ComponentUpdateStatus::Added){
            if (camera){
                cameras.push_back(camera);
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
            }
            if (component->isUpdateable()){
                auto pos = find(updatable.begin(), updatable.end(), component);
                if (pos != updatable.end()){
                    updatable.erase(pos);
                }
            }
        }

    }
}