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
              viewportListener{Engine::instance->getContext()->contextSurfaceSize.createListener([&](ivec2 viewport) {
                  resetProjectionMatrix();
              })}
    {
        resetProjectionMatrix();
    }

    float CameraPerspective::getNear() const {
        return near;
    }

    void CameraPerspective::setNear(float near) {
        CameraPerspective::near = near;
        resetProjectionMatrix();
    }

    float CameraPerspective::getFar() const {
        return far;
    }

    void CameraPerspective::setFar(float far) {
        CameraPerspective::far = far;
        resetProjectionMatrix();
    }

    float CameraPerspective::getFieldOfView() const {
        return fieldOfView;
    }

    void CameraPerspective::setFieldOfView(float fieldOfView) {
        CameraPerspective::fieldOfView = fieldOfView;
        resetProjectionMatrix();
    }

    void CameraPerspective::set(float near, float far, float fieldOfView) {
        this->near = near;
        this->far = far;
        this->fieldOfView = fieldOfView;
        resetProjectionMatrix();
    }

    void CameraPerspective::update(ivec2 viewportDimension) {
        vec2 dim = normalizedViewportDim * (vec2)viewportDimension;
        float aspect = dim.x/dim.y;
        projectionMatrix = perspective(fieldOfView, aspect, near, far);
    }

    void CameraPerspective::resetProjectionMatrix() {
        ivec2 viewportDimension = Engine::instance->getContext()->getContextSurfaceDim();
        update(viewportDimension);
    }
}