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

        float near() const;
        void setNear(float near);
        float far() const;
        void setFar(float far);
        float left() const;
        void setLeft(float left);
        float right() const;
        void setRight(float right);
        float bottom() const;
        void setBottom(float bottom);
        float top() const;
        void setTop(float top);
        void set(float near, float far, float left, float right, float bottom, float top);
        virtual void resetProjectionMatrix() override;
    private:
        float mNear = -1;
        float mFar = 1;
        float mLeft = -1;
        float mRight = 1;
        float mBottom = -1;
        float mTop = 1;
    };
}


