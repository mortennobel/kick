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

#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace kick {
    class GameObject;
    class Transform;
    
    typedef std::vector<std::shared_ptr<Transform>>::iterator TransformIter;
    typedef std::vector<std::shared_ptr<Transform>>::const_iterator ConstTransformIter;
    
    class Transform : public Component {
    public:
        Transform(GameObject *gameObject);
        void setPosition(glm::vec3 position);
        glm::vec3 position();
        void setLocalPosition(glm::vec3 position);
        glm::vec3 localPosition();
        void setLocalRotationEuler(glm::vec3 rot);
        glm::vec3 localRotationEuler();
        void setRotationEuler(glm::vec3 rot);
        glm::vec3 rotationEuler();
        void setRotation(glm::quat rot);
        glm::quat rotation();
        void setLocalRotation(glm::quat rot);
        glm::quat localRotation();
        void setLocalScale(glm::vec3 scale);
        glm::vec3 localScale();
        void setParent(std::shared_ptr<Transform> parent);
        void lookAt(glm::vec3 target, glm::vec3 up = glm::vec3{0,1,0});
        void lookAt(Transform *target, glm::vec3 up = glm::vec3{0,1,0});
        glm::mat4 localMatrix();
        glm::mat4 globalMatrix();
        glm::mat4 localTRSInverse();
        glm::mat4 globalTRSInverse();
		std::shared_ptr<Transform> root();
        std::shared_ptr<Transform> parent();
        TransformIter begin();
        TransformIter end();
        ConstTransformIter begin() const;
        ConstTransformIter end() const;

        glm::vec3 forward();
        glm::vec3 up();
        glm::vec3 right();
    private:
        void markGlobalDirty();
        void markLocalDirty();
        glm::mat4 mLocalMatrix = glm::mat4(0);
        glm::mat4 mGlobalMatrix = glm::mat4(0);
        glm::mat4 mLocalMatrixInverse = glm::mat4(0);
        glm::mat4 mGlobalMatrixInverse = glm::mat4(0);
        glm::vec3 mLocalPosition = glm::vec3(0);
        glm::vec3 mGlobalPosition = glm::vec3(0);
        glm::quat mGlobalRotationQuat = glm::quat(1,0,0,0);
        glm::quat mLocalRotationQuat = glm::quat(1,0,0,0);
        glm::vec3 mLocalScale = glm::vec3(1);
        struct {
            bool local = true;
            bool localInv = true;
            bool global = true;
            bool globalInv = true;
            bool globalPos = true;
            bool globalRot = true;
        } mDirty;
        std::vector<std::shared_ptr<Transform>> mChildren;
        std::shared_ptr<Transform> mParent = nullptr;
    };
}