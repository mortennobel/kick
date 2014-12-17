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
    
    GameObject::GameObject(const string &name, Scene* scene, int uniqueId)
    : mName(name), mScene{scene}, mUniqueId{uniqueId}, mComponents{}, componentListeners{}
    {
        mTransform = addComponent<Transform>();
    }
    
    GameObject::GameObject(const GameObject& other)
        : mName(other.mName), mComponents(other.mComponents), componentListeners(other.componentListeners){
    }
 
    GameObject::~GameObject(){
        for (auto p : mComponents){
            delete p;
        }
    }
    
    GameObject::GameObject(GameObject&& other)
    : mName(move(other.mName)), mComponents(move(other.mComponents)), componentListeners(move(other.componentListeners)){
        other.mDestroyed = true;
    }
    
    GameObject& GameObject::operator=(GameObject&& other) {
        if (this != &other)
        {
            mName = move(other.mName);
            mComponents = move(other.mComponents);
            componentListeners = move(other.componentListeners);
            other.mDestroyed = true;
        }
        return *this;
    }
    
    bool GameObject::destroyComponent(Component *component){
        auto pos = find(mComponents.begin(), mComponents.end(), component);
        if (pos != mComponents.end()){
            component->deactivated();
            componentEvent.notifyListeners({component, ComponentUpdateStatus::Destroyed});
            mComponents.erase(pos);
            delete component;
            return true;
        }
        return false;
    }
    
    ComponentIter GameObject::begin() {
        return mComponents.begin();
    }
    
    ComponentIter GameObject::end() {
        return mComponents.end();
    }
    
    ConstComponentIter GameObject::begin() const{
        return mComponents.begin();
    }
    
    ConstComponentIter GameObject::end() const{
        return mComponents.end();
    }
    
    std::string GameObject::name() const {
        return mName;
    }
    
    void GameObject::setName(std::string str){
        mName = str;
    }

    int GameObject::layer() const {
        return mLayer;
    }

    void GameObject::setLayer(int layer) {
        if (GameObject::mLayer != layer){
            GameObject::mLayer = layer;
            for (auto c : mComponents){
                componentEvent.notifyListeners({c, ComponentUpdateStatus::Updated});
            }
        }
    }

    int GameObject::uniqueId() {
        return mUniqueId;
    }
};