//
//  transform.h
//  KickCPP
//
//  Created by morten on 8/13/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once


#include "component.h"
#include <vector>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace kick {
    class GameObject;
    class Transform;
    
    typedef std::vector<Transform*>::iterator TransformIter;
    typedef std::vector<Transform*>::const_iterator ConstTransformIter;
    
    class Transform : public Component {
    public:
        Transform(GameObject *gameObject);
        void setPosition(glm::vec3 position);
        glm::vec3 getPosition();
        void setLocalPosition(glm::vec3 position);
        glm::vec3 getLocalPosition();
        void setLocalRotationEuler(glm::vec3 rot);
        glm::vec3 getLocalRotationEuler();
        void setRotationEuler(glm::vec3 rot);
        glm::vec3 getRotationEuler();
        void setRotation(glm::quat rot);
        glm::quat getRotation();
        void setLocalRotation(glm::quat rot);
        glm::quat getLocalRotation();
        void setLocalScale(glm::vec3 scale);
        glm::vec3 getLocalScale();
        void setParent(Transform *parent);
        void lookAt(Transform *target, glm::vec3 up = glm::vec3{0,1,0});
        glm::mat4 getLocalMatrix();
        glm::mat4 getGlobalMatrix();
        glm::mat4 getLocalTRSInverse();
        glm::mat4 getGlobalTRSInverse();
        Transform* getParent();
        TransformIter begin();
        TransformIter end();
        ConstTransformIter begin() const;
        ConstTransformIter end() const;
    private:
        void markGlobalDirty();
        void markLocalDirty();
        glm::mat4 localMatrix = glm::mat4(0);
        glm::mat4 globalMatrix = glm::mat4(0);
        glm::mat4 localMatrixInverse = glm::mat4(0);
        glm::mat4 globalMatrixInverse = glm::mat4(0);
        glm::vec3 localPosition = glm::vec3(0);
        glm::vec3 globalPosition = glm::vec3(0);
        glm::quat globalRotationQuat = glm::quat(1,0,0,0);
        glm::quat localRotationQuat = glm::quat(1,0,0,0);
        glm::vec3 localScale = glm::vec3(1);
        struct {
            bool local = true;
            bool localInv = true;
            bool global = true;
            bool globalInv = true;
            bool globalPos = true;
            bool globalRot = true;
        } dirty;
        std::vector<Transform*> children;
        Transform* parent = nullptr;
    };
}