//
// Created by morten on 24/05/14.
//

#include "camera_perspective.h"
#include "engine.h"
#include "kick/scene/transform.h"

using namespace glm;

namespace kick {

    CameraPerspective::CameraPerspective(GameObject *gameObject)
            : Camera(gameObject),
              viewportListener{Engine::instance->getContext()->viewPort.createListener([&](ivec2 viewport) {
                  update();
              })}
    {
        update();
    }

    float CameraPerspective::getNear() const {
        return near;
    }

    void CameraPerspective::setNear(float near) {
        CameraPerspective::near = near;
        update();
    }

    float CameraPerspective::getFar() const {
        return far;
    }

    void CameraPerspective::setFar(float far) {
        CameraPerspective::far = far;
        update();
    }

    float CameraPerspective::getFieldOfView() const {
        return fieldOfView;
    }

    void CameraPerspective::setFieldOfView(float fieldOfView) {
        CameraPerspective::fieldOfView = fieldOfView;
        update();
    }

    void CameraPerspective::set(float near, float far, float fieldOfView) {
        this->near = near;
        this->far = far;
        this->fieldOfView = fieldOfView;
        update();
    }

    void CameraPerspective::update() {
        ivec2 viewportDimension = Engine::instance->getContext()->getViewportDim();
        update(viewportDimension);
    }

    void CameraPerspective::update(ivec2 viewportDimension) {
        vec2 dim = normalizedViewportDim * (vec2)viewportDimension;
        float aspect = dim.x/dim.y;
        projectionMatrix = perspective(fieldOfView, aspect, near, far);
    }
}