//
//  GameObject.cpp
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 5/21/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/scene/game_object.h"
#include "kick/scene/transform.h"
#include "kick/scene/scene.h"

using namespace std;

namespace kick {
    
    GameObject::GameObject(const string &name, Scene* scene)
    :name(name), scene(scene), components(),componentListeners()

    {
        transform = addComponent<Transform>();
    }
    
    GameObject::GameObject(const GameObject& other)
        :name(other.name), components(other.components), componentListeners(other.componentListeners){
    }
 
    GameObject::~GameObject(){
        for (auto p : components){
            delete p;
        }
    }
    
    GameObject::GameObject(GameObject&& other)
    :name(move(other.name)), components(move(other.components)), componentListeners(move(other.componentListeners)){
        other.destroyed = true;
    }
    
    GameObject& GameObject::operator=(GameObject&& other) {
        if (this != &other)
        {
            name = move(other.name);
            components = move(other.components);
            componentListeners = move(other.componentListeners);
            other.destroyed = true;
        }
        return *this;
    }
    
    bool GameObject::destroyComponent(Component *component){
        auto pos = find(components.begin(), components.end(), component);
        if (pos != components.end()){
            component->deactivated();
            componentEvent.notifyListeners({component, ComponentUpdateStatus::Destroyed});
            components.erase(pos);
            delete component;
            return true;
        }
        return false;
    }
    
    ComponentIter GameObject::begin() {
        return components.begin();
    }
    
    ComponentIter GameObject::end() {
        return components.end();
    }
    
    ConstComponentIter GameObject::begin() const{
        return components.begin();
    }
    
    ConstComponentIter GameObject::end() const{
        return components.end();
    }
    
    std::string GameObject::getName() const {
        return name;
    }
    
    void GameObject::setName(std::string str){
        name = str;
    }

    int GameObject::getLayer() const {
        return layer;
    }

    void GameObject::setLayer(int layer) {
        if (GameObject::layer != layer){
            GameObject::layer = layer;
            for (auto c : components){
                componentEvent.notifyListeners({c, ComponentUpdateStatus::Updated});
            }
        }
    }
};