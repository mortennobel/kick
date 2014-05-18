//
//  camera.cpp
//  KickCPP
//
//  Created by morten on 8/12/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/scene/camera.h"
#include <iostream>
#include "kick/core/kickgl.h"
#include <glm/ext.hpp>
#include <cmath>
#include <algorithm>
#include "kick/scene/game_object.h"
#include "kick/scene/transform.h"
#include "kick/scene/scene.h"

using namespace std;
using namespace glm;

namespace kick {
    
    Camera::Camera(GameObject *gameObject)
    :Component(gameObject, false, false), clearFlag(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT){
        cameraValues.perspective.near = 0.1;
        cameraValues.perspective.far = 1000.0;
        cameraValues.perspective.fieldOfView = 60;
    }
    
    void Camera::activated(){
        Scene* scene = gameObject->getScene();
        componentListener = scene->componentEvents.createListener([&](pair<Component*, ComponentUpdateStatus> value){
            if (value.second == ComponentUpdateStatus::Added){
                if (includeComponent(value.first)){
                    renderableComponents.push_back(value.first);
                }
            } else if (value.second == ComponentUpdateStatus::Removed){
                auto iter = find(renderableComponents.begin(),renderableComponents.end(), value.first);
                if (iter != renderableComponents.end()){
                    renderableComponents.erase(iter);
                }
            }
        });
        rebuildComponentList();
    }
    
    void Camera::rebuildComponentList(){
        renderableComponents.clear();
        for (auto & gameObject : *gameObject->getScene()) {
            for (auto & component :  *gameObject){
                if (includeComponent(component)){
                    renderableComponents.push_back(component);
                }
            }
        }
    }
    
    bool Camera::includeComponent(Component* component){
        if (!component->isRenderable()){
            return false;
        }
        if (dynamic_cast<Camera*>(component)){
            return false;
        }
        // todo - filter
        return true;
    }
    
    void Camera::deactivated(){
        // remove listener
        componentListener = {};
    }
    
    glm::vec4 Camera::getClearColor(){
        return clearColor;
    }
    
    void Camera::setClearColor(glm::vec4 clearColor){
        this->clearColor = clearColor;
    }
    
    void Camera::setupViewport(vec2 &offset, vec2 &dim){
        offset = round(offset);
        dim = round(dim);
        int offsetX = (int)offset.x;
        int offsetY = (int)offset.y;
        int width = (int)dim.x;
        int height = (int)dim.y;
        glViewport(offsetX, offsetY, width, height);
        glScissor(offsetX, offsetY, width, height);
    }
    
    void Camera::setupCamera(EngineUniforms *engineUniforms) {
        vec2 viewportDimension = (vec2)/*renderTarget? renderTarget.dimension : */ engineUniforms->viewportDimension.getValue();
        vec2 offset = viewportDimension * normalizedViewportOffset;
        vec2 dim = viewportDimension * normalizedViewportDim;
        setupViewport(offset, dim);
        if (clearFlag) {
            if (isClearColor()) {
                glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
            }
            glClear(clearFlag);
        }
        mat4 projectionMatrix;
        if (cameraType == CameraType::Perspective) {
            projectionMatrix = glm::perspectiveFov(cameraValues.perspective.fieldOfView, viewportDimension.x, viewportDimension.y, cameraValues.perspective.near, cameraValues.perspective.far);
        } else {
            projectionMatrix = ortho(
                cameraValues.orthographic.left, cameraValues.orthographic.right,
                cameraValues.orthographic.bottom, cameraValues.orthographic.top,
                cameraValues.orthographic.near, cameraValues.orthographic.far);
        }
        engineUniforms->viewMatrix = gameObject->getTransform()->getGlobalTRSInverse();
        engineUniforms->viewProjectionMatrix = projectionMatrix * engineUniforms->viewMatrix;
        engineUniforms->projectionMatrix = projectionMatrix;
    }
    
    void Camera::render(EngineUniforms *engineUniforms){
        setupCamera(engineUniforms);
        engineUniforms->sceneLights->recomputeLight(engineUniforms->viewMatrix);
        for (auto c : renderableComponents){
            c->render(engineUniforms);
        }
    }
    
    void Camera::setClearColor(bool clear){
        if (clear){
            clearFlag |= GL_COLOR_BUFFER_BIT;
        } else {
            clearFlag &= ~GL_COLOR_BUFFER_BIT;
        }
    }
    
    bool Camera::isClearColor(){
        return clearFlag & GL_COLOR_BUFFER_BIT;
    }
    
    void Camera::setClearDepth(bool clear){
        if (clear){
            clearFlag |= GL_DEPTH_BUFFER_BIT;
        } else {
            clearFlag &= ~GL_DEPTH_BUFFER_BIT;
        }
    }
    
    bool Camera::isClearDepth(){
        return clearFlag & GL_DEPTH_BUFFER_BIT;
    }
    
    void Camera::setClearStencil(bool clear){
        if (clear){
            clearFlag |= GL_STENCIL_BUFFER_BIT;
        } else {
            clearFlag &= ~GL_STENCIL_BUFFER_BIT;
        }
    }
    
    bool Camera::isClearStencil(){
        return clearFlag & GL_STENCIL_BUFFER_BIT;
    }


    void Camera::setCameraType(CameraType cameraType) { this->cameraType = cameraType; }

    CameraType Camera::getCameraType() { return cameraType;}

    void Camera::setNear(float near) { cameraValues.perspective.near = near; }

    float Camera::getNear() { return cameraValues.perspective.near; }

    void Camera::setFar(float far) { cameraValues.perspective.far = far;}

    float Camera::getFar() { return cameraValues.perspective.far; }

    void Camera::setFieldOfView(float fov) { cameraValues.perspective.fieldOfView = fov; }

    float Camera::getFieldOfView() { return cameraValues.perspective.fieldOfView; }

    void Camera::setLeft(float left) { cameraValues.orthographic.left = left; }

    float Camera::getLeft() { return cameraValues.orthographic.left; }

    void Camera::setRight(float right) { cameraValues.orthographic.right = right;}

    float Camera::getRight() { return cameraValues.orthographic.right; }

    void Camera::setBottom(float bottom) { cameraValues.orthographic.bottom = bottom;}

    float Camera::getBottom() { return cameraValues.orthographic.bottom; }

    void Camera::setTop(float top) { cameraValues.orthographic.top = top; }

    float Camera::getTop() { return cameraValues.orthographic.top; }
}