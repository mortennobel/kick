//
// Created by morten on 24/05/14.
//


#include "camera.h"
#include "game_object.h"

#pragma once


namespace kick {
    class CameraOrthographic : public Camera {
    public:
        CameraOrthographic(GameObject *gameObject);

        float getNear() const;
        void setNear(float near);
        float getFar() const;
        void setFar(float far);
        float getLeft() const;
        void setLeft(float left);
        float getRight() const;
        void setRight(float right);
        float getBottom() const;
        void setBottom(float bottom);
        float getTop() const;
        void setTop(float top);
        void set(float near, float far, float left, float right, float bottom, float top);
        virtual void resetProjectionMatrix() override;
        virtual Ray screenPointToRay(glm::vec2 point) override;
    private:
        float mNear = -1;
        float mFar = 1;
        float mLeft = -1;
        float mRight = 1;
        float mBottom = -1;
        float mTop = 1;
    };
}


