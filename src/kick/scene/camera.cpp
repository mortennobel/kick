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
#include "kick/scene/component_renderable.h"
#include "kick/texture/texture_render_target.h"
#include "kick/core/engine.h"
#include "kick/scene/light.h"
#include "kick/math/misc.h"
#include "kick/material/material.h"

using namespace std;
using namespace glm;

namespace kick {
    
    Camera::Camera(GameObject *gameObject)
    :Component(gameObject), clearFlag(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT),
     projectionMatrix{1}{

    }

    Camera::~Camera() {
        destroyShadowMap();
        delete pickingRenderTarget;
    }
    
    void Camera::activated(){
        Scene* scene = gameObject->getScene();
        componentListener = scene->componentEvents.createListener([&](pair<Component*, ComponentUpdateStatus> value){
            if (value.second == ComponentUpdateStatus::Created){
                auto r = includeComponent(value.first);
                if (r){
                    renderableComponents.push_back(r);
                }
            } else if (value.second == ComponentUpdateStatus::Destroyed){
                auto r = includeComponent(value.first);
                if (r) {
                    auto iter = find(renderableComponents.begin(), renderableComponents.end(), value.first);
                    if (iter != renderableComponents.end()) {
                        renderableComponents.erase(iter);
                    }
                }
            }
        });
        rebuildComponentList();
    }
    
    void Camera::rebuildComponentList(){
        renderableComponents.clear();
        for (auto & gameObject : *gameObject->getScene()) {
            for (auto & component :  *gameObject){
                auto r = includeComponent(component);
                if (r){
                    renderableComponents.push_back(r);
                }
            }
        }
    }
    
    ComponentRenderable *Camera::includeComponent(Component* component){
        auto r = dynamic_cast<ComponentRenderable*>(component);
        if (!r){
            return nullptr;
        }
        if (dynamic_cast<Camera*>(r)){
            return nullptr;
        }
        // todo - filter
        return r;
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

        engineUniforms->viewMatrix = getViewMatrix();
        engineUniforms->viewProjectionMatrix = projectionMatrix * engineUniforms->viewMatrix;
        engineUniforms->projectionMatrix = projectionMatrix;
    }

    glm::mat4 Camera::getViewMatrix(){
        return gameObject->getTransform()->getGlobalTRSInverse();
    }

    void Camera::renderShadowMap(Light* directionalLight){

    }
    
    void Camera::render(EngineUniforms *engineUniforms){
        if (!isEnabled()){
            return;
        }
        auto sceneLights = engineUniforms->sceneLights;
        if (shadow && sceneLights->directionalLight && sceneLights->directionalLight->getShadowType() != ShadowType::None) {
            renderShadowMap(sceneLights->directionalLight);
        }

        if (target){
            target->bind();
        }
        setupCamera(engineUniforms);
        engineUniforms->sceneLights->recomputeLight(engineUniforms->viewMatrix);
        sort(renderableComponents.begin(), renderableComponents.end(), [](ComponentRenderable* r1, ComponentRenderable* r2){
            return r1->getRenderOrder() < r2->getRenderOrder();
        });



        for (auto c : renderableComponents){
            if (c->getGameObject()->getLayer() & cullingMask) {
                c->render(engineUniforms, replacementMaterial.get());
            }
        }
        if (target){
            target->unbind();
        }

        if (pickQueue.size() > 0){
            handleObjectPicking(engineUniforms);
        }
    }

    void Camera::handleObjectPicking(EngineUniforms *engineUniforms) {
        auto viewportSize = engineUniforms->viewportDimension.getValue();
        if (pickingRenderTarget == nullptr || viewportSize != pickingRenderTarget->getSize()){
            delete pickingRenderTarget;
            pickingRenderTarget = new TextureRenderTarget();
            pickingRenderTarget->setSize(viewportSize);
            if (pickingTexture == nullptr){
                pickingTexture = make_shared<Texture2D>();
            }
            ImageFormat imageFormat{};
            imageFormat.mipmap = Mipmap::None;
            pickingTexture->setData(viewportSize.x, viewportSize.y, nullptr, imageFormat);

            pickingRenderTarget->setColorTexture(0, pickingTexture);
            if (pickingMaterial.get() == nullptr){
                pickingMaterial = std::shared_ptr<Material>(new Material());
                pickingMaterial->setShader(Project::loadShader("assets/shaders/__pick.shader"));
            }
            pickingRenderTarget->apply();
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glPixelStorei(GL_PACK_ALIGNMENT, 1);
        }
        pickingRenderTarget->bind();
        glClearColor(0, 0, 0, 0);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        for (auto c : renderableComponents){
            if (c->getGameObject()->getLayer() & cullingMask) {
                c->render(engineUniforms, pickingMaterial.get());
            }
        }
        for (auto q : pickQueue){
            vector<glm::u8vec4> data((int)(q.size.x * q.size.y));
            glReadPixels(q.point.x, q.point.y, q.size.x, q.size.y, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid *) glm::value_ptr(*data.data()));
            std::map<int,int> gameObjectFrequency;
            for (auto p : data){
                int uid = vec4ToUint32(p);
                if (uid != 0){
                    auto iter = gameObjectFrequency.find(uid);
                    if (iter != gameObjectFrequency.end()){
                        iter->second++;
                    } else {
                        gameObjectFrequency.insert(pair<int,int>(uid,1));
                    }
                }
            }
            auto scene = gameObject->getScene();
            for (auto uid : gameObjectFrequency){
                auto hitGameObject = scene->getGameObjectByUID(uid.first);
                if (hitGameObject) {
                    q.onPicked(hitGameObject, uid.second);
                }
            }
        }
        pickingRenderTarget->unbind();
        pickQueue.clear();
    }

    void Camera::setClearColor(bool clear){
        if (clear){
            clearFlag |= GL_COLOR_BUFFER_BIT;
        } else {
            clearFlag &= ~GL_COLOR_BUFFER_BIT;
        }
    }
    
    bool Camera::isClearColor(){
        return (bool) (clearFlag & GL_COLOR_BUFFER_BIT);
    }
    
    void Camera::setClearDepth(bool clear){
        if (clear){
            clearFlag |= GL_DEPTH_BUFFER_BIT;
        } else {
            clearFlag &= ~GL_DEPTH_BUFFER_BIT;
        }
    }
    
    bool Camera::isClearDepth(){
        return (bool) (clearFlag & GL_DEPTH_BUFFER_BIT);
    }
    
    void Camera::setClearStencil(bool clear){
        if (clear){
            clearFlag |= GL_STENCIL_BUFFER_BIT;
        } else {
            clearFlag &= ~GL_STENCIL_BUFFER_BIT;
        }
    }
    
    bool Camera::isClearStencil(){
        return (bool) (clearFlag & GL_STENCIL_BUFFER_BIT);
    }

    glm::mat4 Camera::getProjectionMatrix() {
        return projectionMatrix;
    }

    void Camera::setProjectionMatrix(glm::mat4 projectionMatrix) {
        this->projectionMatrix = projectionMatrix;
    }

    void Camera::resetProjectionMatrix(){
    }

    bool Camera::isShadow() const {
        return shadow;
    }

    void Camera::setShadow(bool renderShadow) {
        if (Camera::shadow != renderShadow) {
            Camera::shadow = renderShadow;
            if (renderShadow){
                initShadowMap();
            } else {
                destroyShadowMap();
            }
        }
    }

    void Camera::initShadowMap() {
        shadowMapShader = Project::loadShader("assets/shaders/__shadowmap.shader");
        shadowMapMaterial = new Material();
        shadowMapMaterial->setShader(shadowMapShader);
    }

    void Camera::destroyShadowMap() {
    }

    int Camera::getCullingMask() const {
        return cullingMask;
    }

    void Camera::setCullingMask(int cullingMask) {
        Camera::cullingMask = cullingMask;
    }

    TextureRenderTarget *Camera::getTarget() const {
        return target;
    }

    void Camera::setTarget(TextureRenderTarget *target) {
        Camera::target = target;
    }

    void Camera::pick(glm::ivec2 point, std::function<void(GameObject*,int)> onPicked, glm::ivec2 size){
        pickQueue.push_back({point, size, onPicked});
    }

    std::shared_ptr<Material> const &Camera::getReplacementMaterial() const {
        return replacementMaterial;
    }

    void Camera::setReplacementMaterial(std::shared_ptr<Material> const &replacementMaterial) {
        Camera::replacementMaterial = replacementMaterial;
    }
}