//
//  camera.cpp
//  KickCPP
//
//  Created by morten on 8/12/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/scene/camera.h"
#include <iostream>
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
#include "kick/core/debug.h"

using namespace std;
using namespace glm;

namespace kick {
    
    Camera::Camera(GameObject *gameObject)
    :Component(gameObject){
        Scene* scene = gameObject->scene();
        componentListener = scene->componentEvents.createListener([&](pair<Component*, ComponentUpdateStatus> value){
            if (value.second == ComponentUpdateStatus::Created){
                auto r = includeComponent(value.first);
                if (r){
                    mRenderableComponents.push_back(r);
                }
            } else if (value.second == ComponentUpdateStatus::Destroyed){
                auto r = includeComponent(value.first);
                if (r) {
                    auto iter = find(mRenderableComponents.begin(), mRenderableComponents.end(), value.first);
                    if (iter != mRenderableComponents.end()) {
                        mRenderableComponents.erase(iter);
                    }
                }
            }
        });
        rebuildComponentList();
    }

    Camera::~Camera() {
        destroyShadowMap();
        delete mPickingRenderTarget;
    }
    

    
    void Camera::rebuildComponentList(){
        mRenderableComponents.clear();
        for (auto & gameObject_ : *gameObject()->scene()) {
            for (auto & component :  *gameObject_){
                auto r = includeComponent(component);
                if (r){
                    mRenderableComponents.push_back(r);
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
    
    glm::vec4 Camera::clearColor(){
        return mClearColor;
    }
    
    void Camera::setClearColor(glm::vec4 clearColor){
        this->mClearColor = clearColor;
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
        vec2 offset = viewportDimension * mNormalizedViewportOffset;
        vec2 dim = viewportDimension * mNormalizedViewportDim;
        setupViewport(offset, dim);
        if (mClearFlag) {
            if (clearColorBuffer()) {
                glClearColor(mClearColor.r, mClearColor.g, mClearColor.b, mClearColor.a);
            }
            glClear(mClearFlag);
        }

        engineUniforms->viewMatrix = viewMatrix();
        engineUniforms->viewProjectionMatrix = mProjectionMatrix * engineUniforms->viewMatrix;
        engineUniforms->projectionMatrix = mProjectionMatrix;
    }

    glm::mat4 Camera::viewMatrix(){
        return mGameObject->transform()->globalTRSInverse();
    }

    void Camera::renderShadowMap(Light* directionalLight){

    }
    
    void Camera::render(EngineUniforms *engineUniforms){
        if (!enabled()){
            return;
        }
        auto sceneLights = engineUniforms->sceneLights;
        engineUniforms->currentCameraTransform = transform();

        if (mShadow && sceneLights->directionalLight && sceneLights->directionalLight->shadowType() != ShadowType::None) {
            renderShadowMap(sceneLights->directionalLight);
        }

        if (mTarget){
            mTarget->bind();
        }
        bool customViewport = mNormalizedViewportOffset != vec2{0} || mNormalizedViewportDim != vec2{1};
        if (customViewport){
            glEnable(GL_SCISSOR_TEST);
        }
        setupCamera(engineUniforms);
        engineUniforms->sceneLights->recomputeLight(engineUniforms->viewMatrix);
        sort(mRenderableComponents.begin(), mRenderableComponents.end(), [](ComponentRenderable* r1, ComponentRenderable* r2){
            return r1->renderOrder() < r2->renderOrder();
        });

        for (auto c : mRenderableComponents){
            if (c->gameObject()->layer() & mCullingMask) {
                c->render(engineUniforms, mReplacementMaterial.get());
            }
        }
        if (mTarget){
            mTarget->unbind();
        }

        if (mPickQueue.size() > 0){
            handleObjectPicking(engineUniforms);
        }
        if (customViewport){
            glDisable(GL_SCISSOR_TEST);
        }
    }

    void Camera::handleObjectPicking(EngineUniforms *engineUniforms) {
        auto viewportSize = engineUniforms->viewportDimension.getValue();
        if (mPickingRenderTarget == nullptr || viewportSize != mPickingRenderTarget->size()){
            delete mPickingRenderTarget;
            mPickingRenderTarget = new TextureRenderTarget();
            mPickingRenderTarget->setSize(viewportSize);
            if (mPickingTexture == nullptr){
                mPickingTexture = make_shared<Texture2D>();
            }
            ImageFormat imageFormat{};
            imageFormat.mipmap = Mipmap::None;
            TextureSampler sampler{};
            sampler.filterMag = TextureFilter::Nearest;
            sampler.filterMin = TextureFilter::Nearest;
            sampler.wrapS = TextureWrap::ClampToEdge;
            sampler.wrapT = TextureWrap::ClampToEdge;
            mPickingTexture->setTextureSampler(sampler);
            mPickingTexture->setData(viewportSize.x, viewportSize.y, nullptr, imageFormat);

            mPickingRenderTarget->setColorTexture(0, mPickingTexture);
            if (mPickingMaterial.get() == nullptr){
                mPickingMaterial = std::shared_ptr<Material>(new Material());
                mPickingMaterial->setShader(Project::loadShader("assets/shaders/__pick.shader"));
            }
            mPickingRenderTarget->apply();
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glPixelStorei(GL_PACK_ALIGNMENT, 1);
        }
        mPickingRenderTarget->bind();
        glClearColor(0, 0, 0, 0);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        for (auto c : mRenderableComponents){
            if (c->gameObject()->layer() & mCullingMask) {
                c->render(engineUniforms, mPickingMaterial.get());
            }
        }
        for (auto q : mPickQueue){
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
            auto scene = mGameObject->scene();
            for (auto uid : gameObjectFrequency){
                auto hitGameObject = scene->gameObjectByUID(uid.first);
                if (hitGameObject) {
                    q.onPicked(hitGameObject, uid.second);
                }
            }
        }
        mPickingRenderTarget->unbind();
        mPickQueue.clear();
    }

    void Camera::setClearColorBuffer(bool clear){
        if (clear){
            mClearFlag |= GL_COLOR_BUFFER_BIT;
        } else {
            mClearFlag &= ~GL_COLOR_BUFFER_BIT;
        }
    }
    
    bool Camera::clearColorBuffer(){
        return (bool) (mClearFlag & GL_COLOR_BUFFER_BIT);
    }
    
    void Camera::setClearDepthBuffer(bool clear){
        if (clear){
            mClearFlag |= GL_DEPTH_BUFFER_BIT;
        } else {
            mClearFlag &= ~GL_DEPTH_BUFFER_BIT;
        }
    }
    
    bool Camera::clearDepthBuffer(){
        return (bool) (mClearFlag & GL_DEPTH_BUFFER_BIT);
    }
    
    void Camera::setClearStencilBuffer(bool clear){
        if (clear){
            mClearFlag |= GL_STENCIL_BUFFER_BIT;
        } else {
            mClearFlag &= ~GL_STENCIL_BUFFER_BIT;
        }
    }
    
    bool Camera::clearStencilBuffer(){
        return (bool) (mClearFlag & GL_STENCIL_BUFFER_BIT);
    }

    glm::mat4 Camera::projectionMatrix() {
        return mProjectionMatrix;
    }

    void Camera::setProjectionMatrix(glm::mat4 projectionMatrix) {
        this->mProjectionMatrix = projectionMatrix;
    }

    void Camera::resetProjectionMatrix(){
    }

    bool Camera::shadow() const {
        return mShadow;
    }

    void Camera::setShadow(bool renderShadow) {
        if (Camera::mShadow != renderShadow) {
            Camera::mShadow = renderShadow;
            if (renderShadow){
                initShadowMap();
            } else {
                destroyShadowMap();
            }
        }
    }

    void Camera::initShadowMap() {
        mShadowMapShader = Project::loadShader("assets/shaders/__shadowmap.shader");
        mShadowMapMaterial = new Material();
        mShadowMapMaterial->setShader(mShadowMapShader);
    }

    void Camera::destroyShadowMap() {
    }

    int Camera::cullingMask() const {
        return mCullingMask;
    }

    void Camera::setCullingMask(int cullingMask) {
        Camera::mCullingMask = cullingMask;
    }

    TextureRenderTarget *Camera::target() const {
        return mTarget;
    }

    void Camera::setTarget(TextureRenderTarget *target) {
        Camera::mTarget = target;
    }

    void Camera::pick(glm::ivec2 point, std::function<void(GameObject*,int)> onPicked, glm::ivec2 size){
        mPickQueue.push_back({point, size, onPicked});
    }

    std::shared_ptr<Material> const &Camera::replacementMaterial() const {
        return mReplacementMaterial;
    }

    void Camera::setReplacementMaterial(std::shared_ptr<Material> const &replacementMaterial) {
        Camera::mReplacementMaterial = replacementMaterial;
    }

    glm::vec2 const &Camera::viewportOffset() const {
        return mNormalizedViewportOffset;
    }

    void Camera::setViewportOffset(glm::vec2 const &normalizedViewportOffset) {
        Camera::mNormalizedViewportOffset = normalizedViewportOffset;
    }

    glm::vec2 const &Camera::viewportDim() const {
        return mNormalizedViewportDim;
    }

    void Camera::setViewportDim(glm::vec2 const &normalizedViewportDim) {
        Camera::mNormalizedViewportDim = normalizedViewportDim;
    }

    Ray Camera::screenPointToRay(glm::vec2 point){

        vec3 zoomDirection = transform()->forward();

        vec2 surfaceDim = (vec2)(Engine::context()->getContextSurfaceDim());
        vec3 mousePosNormalizedNear{(vec2(point) / surfaceDim)*2.0f-vec2(1.0f),1.0f};
        vec3 mousePosNormalizedFar{(vec2(point) / surfaceDim)*2.0f-vec2(1.0f),-1.0f};
        // inverse
        mousePosNormalizedNear.y *=-1;
        mousePosNormalizedFar.y *=-1;

        mat4 invProjection = inverse(projectionMatrix());
        mat4 invView = transform()->globalMatrix();

        vec4 mousePosViewNear = invProjection * vec4(mousePosNormalizedNear, 1.0);
        vec4 mousePosViewFar = invProjection * vec4(mousePosNormalizedFar, 1.0);
        mousePosViewNear /= 1.0f/mousePosViewNear.w;
        mousePosViewFar /= 1.0f/mousePosViewFar.w;
        mousePosViewNear.w = 1;
        mousePosViewFar.w = 1;

        vec4 mousePosWorldNear = invView * mousePosViewNear;
        vec4 mousePosWorldFar = invView * mousePosViewFar;

        return Ray{(vec3)mousePosWorldNear, normalize((vec3)mousePosWorldFar - (vec3)mousePosWorldNear)};
    }
}