//
//  transform.cpp
//  KickCPP
//
//  Created by morten on 8/13/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/scene/transform.h"
#include "kick/math/glm_ext.h"
#include <algorithm>

using namespace glm;

namespace kick {
    Transform::Transform(GameObject *gameObject)
    :Component(gameObject, false, false){
    }
    
    void Transform::markGlobalDirty(){
        dirty.global = 1;
        dirty.globalInv = 1;
        dirty.globalPos = 1;
        dirty.globalRot = 1;
        for (auto c : children){
            c->markGlobalDirty();
        }
    }
    
    void Transform::markLocalDirty(){
        dirty.local = 1;
        dirty.localInv = 1;
        markGlobalDirty();
    }
    
    void Transform::setPosition(glm::vec3 position){
        if (!parent){
            setLocalPosition(position);
            return;
        }
        // todo test for correctness
        vec3 currentPosition = getPosition();
        setLocalPosition(vec3{
            currentPosition.x - position.x,
            currentPosition.y - position.y,
            currentPosition.z - position.z
        });
        markLocalDirty();
    }
    
    glm::vec3 Transform::getPosition(){
        if (parent == nullptr){
            return localPosition;
        }
        if (dirty.globalPos){
            auto pos = getGlobalMatrix() * vec4(0,0,0,1);
            globalPosition = vec3(pos);
        }
        return globalPosition;
    }
    
    void Transform::setLocalPosition(glm::vec3 position){
        localPosition = position;
        markLocalDirty();
    }
    
    glm::vec3 Transform::getLocalPosition(){
        return localPosition;
    }
    
    void Transform::setLocalRotationEuler(glm::vec3 rot){
        vec3 rotRadians = radians(rot);
        localRotationQuat = quat(rotRadians);
        markLocalDirty();
    }
    
    glm::vec3 Transform::getLocalRotationEuler(){
        return degrees(eulerAngles(localRotationQuat));
    }
    
    void Transform::setRotationEuler(glm::vec3 rot){
        vec3 rotationRadians = radians(rot);
        setRotation(quat(rotationRadians));
    }
    
    glm::vec3 Transform::getRotationEuler(){
        return degrees(eulerAngles(getRotation()));
    }
    
    void Transform::setRotation(glm::quat rot){
        if (parent == nullptr){
            localRotationQuat = rot;
        }
        quat diff = inverse(rot) * getRotation();
        setLocalRotation(localRotationQuat * diff);
    }
    
    glm::quat Transform::getRotation(){
        if (parent == nullptr){
            return localRotationQuat;
        }
        if (dirty.globalRot){
            globalRotationQuat = localRotationQuat;
            Transform *parentIterator = this->parent;
            while (parentIterator != nullptr){
                globalRotationQuat = parentIterator->localRotationQuat * globalRotationQuat;
                parentIterator = parentIterator->parent;
            }
        }
        return globalRotationQuat;
    }
    
    void Transform::setLocalRotation(glm::quat rot){
        localRotationQuat = rot;
        markLocalDirty();
    }
    
    glm::quat Transform::getLocalRotation(){
        return localRotationQuat;
    }
    
    void Transform::setLocalScale(glm::vec3 scale){
        localScale = scale;
        markLocalDirty();
    }
    
    glm::vec3 Transform::getLocalScale(){
        return localScale;
    }
    
    void Transform::setParent(Transform *parent){
        if (this->parent){
            auto pos = find(this->parent->children.begin(),this->parent->children.end(), this);
            this->parent->children.erase(pos);
        }
        this->parent = parent;
        if (parent){
            parent->children.push_back(this);
        }
    }
    
    glm::mat4 Transform::getLocalTRSInverse(){
        if (dirty.localInv){
            localMatrixInverse = Math::TRSInverse(localPosition, localRotationQuat, localScale);
            dirty.localInv = false;
        }
        return localMatrixInverse;
    }
    
    glm::mat4 Transform::getGlobalTRSInverse(){
        if (dirty.globalInv){
            globalMatrixInverse = getLocalTRSInverse();
            auto transformIterator = parent;
            while (transformIterator != nullptr){
                globalMatrixInverse = globalMatrixInverse * transformIterator->getLocalTRSInverse();
                transformIterator = transformIterator->parent;
            }
            dirty.globalInv = false;
        }
        return globalMatrixInverse;
    }
    
    void Transform::lookAt(Transform *transform, glm::vec3 up){
        localRotationQuat = Math::lookAt(getPosition(), transform->getPosition(), up);
        markLocalDirty();
    }
    
    glm::mat4 Transform::getLocalMatrix(){
        if (dirty.local) {
            localMatrix = Math::TRS(localPosition, localRotationQuat, localScale);
            dirty.local = false;
        }
        return localMatrix;
    }
    
    glm::mat4 Transform::getGlobalMatrix(){
        if (dirty.global) {
            globalMatrix = getLocalMatrix();
            
            Transform * transformIterator = parent;
            while (transformIterator != nullptr) {
                globalMatrix = transformIterator->getLocalMatrix() * globalMatrix;
                transformIterator  = transformIterator->parent;
            }
            dirty.global = false;
        }
        return globalMatrix;
    }
    
    TransformIter Transform::begin(){
        return children.begin();
    }
    
    TransformIter Transform::end(){
        return children.end();
    }
    
    ConstTransformIter Transform::begin() const {
        return children.begin();
    }
    
    ConstTransformIter Transform::end() const {
        return children.end();
    }

    Transform *Transform::getParent() {
        return parent;
    }
}