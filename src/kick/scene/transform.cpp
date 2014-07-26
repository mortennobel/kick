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
#include <iostream>

using namespace glm;

namespace kick {
    Transform::Transform(GameObject *gameObject)
    :Component(gameObject){
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
        assert(!glm::any(glm::isnan(position)));
        if (!parent){
            setLocalPosition(position);
            return;
        }
        // todo test for correctness
        vec3 currentPosition = getPosition();
        setLocalPosition(currentPosition - position);
        markLocalDirty();
    }
    
    glm::vec3 Transform::getPosition(){
        if (!parent){
            return localPosition;
        }
        if (dirty.globalPos){
            auto pos = getGlobalMatrix() * vec4(0,0,0,1);
            globalPosition = vec3(pos);
        }
        return globalPosition;
    }
    
    void Transform::setLocalPosition(glm::vec3 position){
        assert(!glm::any(glm::isnan(position)));
        localPosition = position;
        markLocalDirty();
    }
    
    glm::vec3 Transform::getLocalPosition(){
        return localPosition;
    }
    
    void Transform::setLocalRotationEuler(glm::vec3 angles){
        assert(!glm::any(glm::isnan(angles)));
        mat4 rot = yawPitchRoll(angles.y, angles.x, angles.z);
        localRotationQuat = quat_cast(rot);
        markLocalDirty();
    }
    
    glm::vec3 Transform::getLocalRotationEuler(){
        return eulerAngles(localRotationQuat);
    }
    
    void Transform::setRotationEuler(glm::vec3 angles){
        assert(!glm::any(glm::isnan(angles)));
        mat4 rot = yawPitchRoll(angles.y, angles.x, angles.z);
        setRotation(quat_cast(rot));
    }
    
    glm::vec3 Transform::getRotationEuler(){
        return eulerAngles(getRotation());
    }
    
    void Transform::setRotation(glm::quat rot){
        assert(!glm::isnan(rot.w) && !glm::isnan(rot.x) && !glm::isnan(rot.y) && !glm::isnan(rot.z));
        if (parent == nullptr){
            setLocalRotation(rot);
        } else {
            quat diff = conjugate(rot) * getRotation();
            setLocalRotation(localRotationQuat * diff);
        }
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
        assert(!glm::isnan(rot.w) && !glm::isnan(rot.x) && !glm::isnan(rot.y) && !glm::isnan(rot.z));
        localRotationQuat = rot;
        markLocalDirty();
    }
    
    glm::quat Transform::getLocalRotation(){
        return localRotationQuat;
    }
    
    void Transform::setLocalScale(glm::vec3 scale){
        assert(!glm::any(glm::isnan(scale)));
        localScale = scale;
        markLocalDirty();
    }
    
    glm::vec3 Transform::getLocalScale(){
        return localScale;
    }
    
    void Transform::setParent(Transform *parent){
        if (parent == this){
            return;
        }
        if (this->parent){
            auto pos = find(this->parent->children.begin(),this->parent->children.end(), this);
            this->parent->children.erase(pos);
        }
        this->parent = parent;
        if (parent){
            parent->children.push_back(this);

            // search for circularity
            while (parent != nullptr){
                if (parent->parent == this){ // circularity found - break
                    parent->setParent(nullptr);
                }
                parent = parent->parent;
            }
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
            while (transformIterator){
                globalMatrixInverse = globalMatrixInverse * transformIterator->getLocalTRSInverse();
                transformIterator = transformIterator->parent;
            }
            dirty.globalInv = false;
        }
        return globalMatrixInverse;
    }
    
    void Transform::lookAt(Transform *target, glm::vec3 up){
        assert(!glm::any(glm::isnan(up)));
        vec3 eye = getPosition();
        vec3 center = target->getPosition();
        assert(glm::length(eye - center) > glm::epsilon<float>());
        auto rotation = Math::lookAt(eye, center, up);
        setRotation(conjugate(rotation));
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
            while (transformIterator) {
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