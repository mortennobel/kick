//
// Created by morten on 24/05/14.
//

#include "camera_orthographic.h"

#include "kick/scene/transform.h"

using namespace glm;

namespace kick {
    CameraOrthographic::CameraOrthographic(GameObject *gameObject)
            : Camera(gameObject)
    {
        resetProjectionMatrix();
    }

    float CameraOrthographic::getNear() const {
        return near;
    }

    void CameraOrthographic::setNear(float near) {
        CameraOrthographic::near = near;
        resetProjectionMatrix();
    }

    float CameraOrthographic::getFar() const {
        return far;
    }

    void CameraOrthographic::setFar(float far) {
        CameraOrthographic::far = far;
        resetProjectionMatrix();
    }

    float CameraOrthographic::getLeft() const {
        return left;
    }

    void CameraOrthographic::setLeft(float left) {
        CameraOrthographic::left = left;
        resetProjectionMatrix();
    }

    float CameraOrthographic::getRight() const {
        return right;
    }

    void CameraOrthographic::setRight(float right) {
        CameraOrthographic::right = right;
        resetProjectionMatrix();
    }

    float CameraOrthographic::getBottom() const {
        return bottom;
    }

    void CameraOrthographic::setBottom(float bottom) {
        CameraOrthographic::bottom = bottom;
        resetProjectionMatrix();
    }

    float CameraOrthographic::getTop() const {
        return top;
    }

    void CameraOrthographic::setTop(float top) {
        CameraOrthographic::top = top;
        resetProjectionMatrix();
    }

    void CameraOrthographic::resetProjectionMatrix() {
        projectionMatrix = ortho(
                left, right,
                bottom, top,
                near, far);
    }

    void CameraOrthographic::set(float near, float far, float left, float right, float bottom, float top) {
        this->near = near;
        this->far = far;
        this->left = left;
        this->right = right;
        this->bottom = bottom;
        this->top = top;
        resetProjectionMatrix();
    }
}