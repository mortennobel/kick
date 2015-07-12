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
        mDirty.global = 1;
        mDirty.globalInv = 1;
        mDirty.globalPos = 1;
        mDirty.globalRot = 1;
        for (auto c : mChildren){
            c->markGlobalDirty();
        }
    }
    
    void Transform::markLocalDirty(){
        mDirty.local = 1;
        mDirty.localInv = 1;
        markGlobalDirty();
    }
    
    void Transform::setPosition(glm::vec3 position_){
        assert(!glm::any(glm::isnan(position_)));
        if (!mParent){
            setLocalPosition(position_);
            return;
        }
        vec3 currentPosition = position();
        setLocalPosition(currentPosition - position_);
        markLocalDirty();
    }
    
    glm::vec3 Transform::position(){
        if (!mParent){
            return mLocalPosition;
        }
        if (mDirty.globalPos){
            auto pos = globalMatrix() * vec4(0,0,0,1);
            mGlobalPosition = vec3(pos);
        }
        return mGlobalPosition;
    }
    
    void Transform::setLocalPosition(glm::vec3 position){
        assert(!glm::any(glm::isnan(position)));
        mLocalPosition = position;
        markLocalDirty();
    }
    
    glm::vec3 Transform::localPosition(){
        return mLocalPosition;
    }
    
    void Transform::setLocalRotationEuler(glm::vec3 angles){
        assert(!glm::any(glm::isnan(angles)));
        mat4 rot = yawPitchRoll(angles.y, angles.x, angles.z);
        mLocalRotationQuat = quat_cast(rot);
        markLocalDirty();
    }
    
    glm::vec3 Transform::localRotationEuler(){
        return eulerAngles(mLocalRotationQuat);
    }
    
    void Transform::setRotationEuler(glm::vec3 angles){
        assert(!glm::any(glm::isnan(angles)));
        mat4 rot = yawPitchRoll(angles.y, angles.x, angles.z);
        setRotation(quat_cast(rot));
    }
    
    glm::vec3 Transform::rotationEuler(){
        return eulerAngles(rotation());
    }
    
    void Transform::setRotation(glm::quat rot){
        assert(!glm::isnan(rot.w) && !glm::isnan(rot.x) && !glm::isnan(rot.y) && !glm::isnan(rot.z));
        if (mParent == nullptr ||
                mParent->mGlobalRotationQuat == glm::quat{1,0,0,0}){ // if parent is identity rotation
            setLocalRotation(rot);
        } else {
            quat diff = conjugate(rot) * rotation();
            setLocalRotation(mLocalRotationQuat * diff);
        }
    }
    
    glm::quat Transform::rotation(){
        if (mParent == nullptr){
            return mLocalRotationQuat;
        }
        if (mDirty.globalRot){
            mGlobalRotationQuat = mLocalRotationQuat;
            auto parentIterator = this->mParent;
            while (parentIterator != nullptr){
                mGlobalRotationQuat = parentIterator->mLocalRotationQuat * mGlobalRotationQuat;
                parentIterator = parentIterator->mParent;
            }
        }
        return mGlobalRotationQuat;
    }
    
    void Transform::setLocalRotation(glm::quat rot){
        assert(!glm::isnan(rot.w) && !glm::isnan(rot.x) && !glm::isnan(rot.y) && !glm::isnan(rot.z));
        mLocalRotationQuat = rot;
        markLocalDirty();
    }
    
    glm::quat Transform::localRotation(){
        return mLocalRotationQuat;
    }
    
    void Transform::setLocalScale(glm::vec3 scale){
        assert(!glm::any(glm::isnan(scale)));
        mLocalScale = scale;
        markLocalDirty();
    }
    
    glm::vec3 Transform::localScale(){
        return mLocalScale;
    }
    
    void Transform::setParent(std::shared_ptr<Transform> parent){
        if (parent && parent.get() == this){
            return;
        }
        if (this->mParent){
            auto pos = find(this->mParent->mChildren.begin(),this->mParent->mChildren.end(), std::dynamic_pointer_cast<Transform>(shared_from_this()));
            this->mParent->mChildren.erase(pos);
        }
        this->mParent = parent;
        if (parent){
            parent->mChildren.push_back(std::dynamic_pointer_cast<Transform>( shared_from_this()));

            // search for circularity
            while (parent != nullptr){
                if (parent->mParent && parent->mParent.get() == this){ // circularity found - break
                    parent->setParent(std::shared_ptr<Transform>());
                }
                parent = parent->mParent;
            }
        }
    }
    
    glm::mat4 Transform::localTRSInverse(){
        if (mDirty.localInv){
            mLocalMatrixInverse = kick::TRSInverse(mLocalPosition, mLocalRotationQuat, mLocalScale);
            mDirty.localInv = false;
        }
        return mLocalMatrixInverse;
    }
    
    glm::mat4 Transform::globalTRSInverse(){
        if (mDirty.globalInv){
            mGlobalMatrixInverse = localTRSInverse();
            auto transformIterator = mParent;
            while (transformIterator){
                mGlobalMatrixInverse = mGlobalMatrixInverse * transformIterator->localTRSInverse();
                transformIterator = transformIterator->mParent;
            }
            mDirty.globalInv = false;
        }
        return mGlobalMatrixInverse;
    }

    void Transform::lookAt(vec3 center, glm::vec3 up){
        assert(!glm::any(glm::isnan(up)));
        vec3 eye = position();
        assert(glm::length(eye - center) > glm::epsilon<float>());
        auto rotation = kick::lookAt(eye, center, up);
        setRotation(conjugate(rotation));
    }

    void Transform::lookAt(Transform *target, glm::vec3 up){
        vec3 center = target->position();
        lookAt(center, up);
    }

    void Transform::lookAt(std::shared_ptr<Transform> target, glm::vec3 up){
        vec3 center = target->position();
        lookAt(center, up);
    }
    
    glm::mat4 Transform::localMatrix(){
        if (mDirty.local) {
            mLocalMatrix = kick::TRS(mLocalPosition, mLocalRotationQuat, mLocalScale);
            mDirty.local = false;
        }
        return mLocalMatrix;
    }
    
    glm::mat4 Transform::globalMatrix(){
        if (mDirty.global) {
            mGlobalMatrix = localMatrix();
            
            auto transformIterator = mParent;
            while (transformIterator) {
                mGlobalMatrix = transformIterator->localMatrix() * mGlobalMatrix;
                transformIterator  = transformIterator->mParent;
            }
            mDirty.global = false;
        }
        return mGlobalMatrix;
    }
    
    TransformIter Transform::begin(){
        return mChildren.begin();
    }
    
    TransformIter Transform::end(){
        return mChildren.end();
    }
    
    ConstTransformIter Transform::begin() const {
        return mChildren.begin();
    }
    
    ConstTransformIter Transform::end() const {
        return mChildren.end();
    }

    std::shared_ptr<Transform> Transform::root() {
		if (!mParent){
			return std::dynamic_pointer_cast<Transform>(shared_from_this());
		}
		auto t = mParent;
		while (t->mParent) {
			t = t->mParent;
		}
		return t;
	}

    std::shared_ptr<Transform> Transform::parent() {
        return mParent;
    }

    vec3 Transform::forward() {
        return mat3_cast(rotation()) * vec3(0,0,-1);
    }

    vec3 Transform::up() {
        return mat3_cast(rotation()) * vec3(0,1,0);
    }

    vec3 Transform::right() {
        return mat3_cast(rotation()) * vec3(1,0,0);
    }
}