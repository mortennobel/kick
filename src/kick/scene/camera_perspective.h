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
        float near() const;
        void setNear(float near);
        float far() const;
        void setFar(float far);
        float fieldOfViewY() const;
        float fieldOfViewX() const;
        void setFieldOfViewY(float fieldOfViewY);
        void set(float near, float far, float fieldOfView);
        virtual void resetProjectionMatrix() override;
        virtual Ray screenPointToRay(glm::vec2 point) override;
    private:
        void update(glm::ivec2 viewportDimension);
        float mNear = 0.1f;
        float mFar = 1000.0f;
        float mFieldOfViewY = glm::radians(60.0f);
        EventListener<glm::ivec2> viewportListener;
    };
}

