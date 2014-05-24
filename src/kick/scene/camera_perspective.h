//
// Created by morten on 24/05/14.
//


#include "kick/scene/camera.h"
#include "kick/scene/game_object.h"
#include "glm/gtc/constants.hpp"

#pragma once

namespace kick {
    class CameraPerspective : public Camera {
    public:
        CameraPerspective(GameObject *gameObject);
        float getNear() const;
        void setNear(float near);
        float getFar() const;
        void setFar(float far);
        float getFieldOfView() const;
        void setFieldOfView(float fieldOfView);
        void set(float near, float far, float fieldOfView);
    private:
        void update(glm::ivec2 viewportDimension);
        void update();
        float near = 0.1f;
        float far = 1000.0f;
        float fieldOfView = glm::radians(60.0f);
        EventListener<glm::ivec2> viewportListener;
    };
}

