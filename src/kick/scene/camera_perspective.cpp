//
// Created by morten on 24/05/14.
//

#include "kick/scene/camera_perspective.h"
#include "kick/core/engine.h"
#include "kick/scene/transform.h"

using namespace glm;

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

    float CameraPerspective::fieldOfView() const {
        return mFieldOfView;
    }

    void CameraPerspective::setFieldOfView(float fieldOfView) {
        CameraPerspective::mFieldOfView = fieldOfView;
        resetProjectionMatrix();
    }

    void CameraPerspective::set(float near, float far, float fieldOfView) {
        this->mNear = near;
        this->mFar = far;
        this->mFieldOfView = fieldOfView;
        resetProjectionMatrix();
    }

    void CameraPerspective::update(ivec2 viewportDimension) {
        vec2 dim = mNormalizedViewportDim * (vec2)viewportDimension;
        float aspect = dim.x/dim.y;
        mProjectionMatrix = perspective(mFieldOfView, aspect, mNear, mFar);
    }

    void CameraPerspective::resetProjectionMatrix() {
        ivec2 viewportDimension = Engine::context()->getContextSurfaceDim();
        update(viewportDimension);
    }
}