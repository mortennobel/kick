//
// Created by morten on 24/05/14.
//

#include "camera_orthographic.h"

#include "kick/scene/transform.h"
#include "log.h"

using namespace glm;

namespace kick {
    CameraOrthographic::CameraOrthographic(GameObject *gameObject)
            : Camera(gameObject)
    {
        resetProjectionMatrix();
    }

    float CameraOrthographic::getNear() const {
        return mNear;
    }

    void CameraOrthographic::setNear(float near) {
        CameraOrthographic::mNear = near;
        resetProjectionMatrix();
    }

    float CameraOrthographic::getFar() const {
        return mFar;
    }

    void CameraOrthographic::setFar(float far) {
        CameraOrthographic::mFar = far;
        resetProjectionMatrix();
    }

    float CameraOrthographic::getLeft() const {
        return mLeft;
    }

    void CameraOrthographic::setLeft(float left) {
        CameraOrthographic::mLeft = left;
        resetProjectionMatrix();
    }

    float CameraOrthographic::getRight() const {
        return mRight;
    }

    void CameraOrthographic::setRight(float right) {
        CameraOrthographic::mRight = right;
        resetProjectionMatrix();
    }

    float CameraOrthographic::getBottom() const {
        return mBottom;
    }

    void CameraOrthographic::setBottom(float bottom) {
        CameraOrthographic::mBottom = bottom;
        resetProjectionMatrix();
    }

    float CameraOrthographic::getTop() const {
        return mTop;
    }

    void CameraOrthographic::setTop(float top) {
        CameraOrthographic::mTop = top;
        resetProjectionMatrix();
    }

    void CameraOrthographic::resetProjectionMatrix() {
        mProjectionMatrix = ortho(
                mLeft, mRight,
                mBottom, mTop,
                mNear, mFar);
    }

    void CameraOrthographic::set(float near, float far, float left, float right, float bottom, float top) {
        this->mNear = near;
        this->mFar = far;
        this->mLeft = left;
        this->mRight = right;
        this->mBottom = bottom;
        this->mTop = top;
        resetProjectionMatrix();
    }

    Ray CameraOrthographic::screenPointToRay(glm::vec2 point) {
        logError("CameraOrthographic::screenPointToRay Not implemented");// todo
        return Ray();
    }
}