//
// Created by morten on 24/05/14.
//

#include "kick/scene/camera_perspective.h"
#include "kick/core/engine.h"
#include "kick/scene/transform.h"
#include <iostream>

using namespace glm;
using namespace std;

namespace kick {

    CameraPerspective::CameraPerspective(GameObject *gameObject)
            : Camera(gameObject),
              viewportListener{Engine::context()->contextSurfaceSize.createListener([&](ivec2 viewport) {
                  resetProjectionMatrix();
              })}
    {
        resetProjectionMatrix();
    }

    float CameraPerspective::near() const {
        return mNear;
    }

    void CameraPerspective::setNear(float near) {
        CameraPerspective::mNear = near;
        resetProjectionMatrix();
    }

    float CameraPerspective::far() const {
        return mFar;
    }

    void CameraPerspective::setFar(float far) {
        CameraPerspective::mFar = far;
        resetProjectionMatrix();
    }

    float CameraPerspective::fieldOfViewY() const {
        return mFieldOfViewY;
    }

    void CameraPerspective::setFieldOfViewY(float fieldOfViewY) {
        CameraPerspective::mFieldOfViewY = fieldOfViewY;
        resetProjectionMatrix();
    }

    void CameraPerspective::set(float near, float far, float fieldOfView) {
        this->mNear = near;
        this->mFar = far;
        this->mFieldOfViewY = fieldOfView;
        resetProjectionMatrix();
    }

    void CameraPerspective::update(ivec2 viewportDimension) {
        vec2 dim = mNormalizedViewportDim * (vec2)viewportDimension;
        float aspect = dim.x/dim.y;
        mProjectionMatrix = perspective(mFieldOfViewY, aspect, mNear, mFar);
    }

    void CameraPerspective::resetProjectionMatrix() {
        ivec2 viewportDimension = Engine::context()->getContextSurfaceDim();
        update(viewportDimension);
    }

    float CameraPerspective::fieldOfViewX() const {
        ivec2 screenSize = Engine::context()->getContextSurfaceDim();
        vec2 viewport = ((vec2 )screenSize)*mNormalizedViewportDim;
        float aspect = viewport.x / viewport.y;
        return mFieldOfViewY * aspect;
    }

    Ray CameraPerspective::screenPointToRay(glm::vec2 point){

        vec3 zoomDirection = transform()->forward();
        float fovY = fieldOfViewY();
        float fovX = fieldOfViewX();

        // pixel perfect rays
        vec2 mousePosNormalized{vec2(0.5f + point) / (vec2)(Engine::context()->getContextSurfaceDim()-ivec2(1))};
//        vec2 mousePosNormalized{vec2(point) / (vec2)(Engine::context()->getContextSurfaceDim())};
        vec2 mousePosClip{mousePosNormalized*2.0f - vec2{1}};

        vec2 mousePosRotationFromCenter{mousePosClip*vec2{fovX/2,fovY/2}};
        auto mat = (mat3)eulerAngleXY(-mousePosRotationFromCenter.y,-mousePosRotationFromCenter.x);
        auto rayDirection = mat * zoomDirection;
        return Ray{transform()->position(), rayDirection};
    }
}