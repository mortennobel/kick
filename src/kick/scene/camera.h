//
//  camera.h
//  KickCPP
//
//  Created by morten on 8/12/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once
#include "component.h"
#include <vector>

#include <glm/glm.hpp>
#include "kick/core/event_listener.h"
#include "kick/math/ray.h"
#include "component_renderable.h"
#include <utility>
#include <functional>

namespace kick {
    class GameObject;
    class Shader;
    class Material;
    class TextureRenderTarget;
    class Texture2D;

    struct PickEntry {
        glm::ivec2 point;
        glm::ivec2 size;
        std::function<void(GameObject*, int)> onPicked;
    };

    class Camera : public Component {
    public:
        Camera(GameObject *gameObject);
        ~Camera();
        virtual void deactivated();
        virtual void render(EngineUniforms *engineUniforms);

        // reset matrix if used parameters (if any)
        virtual void resetProjectionMatrix();
        glm::vec4 clearColor();
        void setClearColor(glm::vec4 clearColor);
        void setClearColorBuffer(bool clear);
        bool clearColorBuffer();
        void setClearDepthBuffer(bool clear);
        bool clearDepthBuffer();
        void setClearStencilBuffer(bool clear);
        bool clearStencilBuffer();
        void setupCamera(EngineUniforms *engineUniforms);
        glm::mat4 projectionMatrix();
        glm::mat4 viewMatrix();
        // override projection matrix
        void setProjectionMatrix(glm::mat4 projectionMatrix);
        bool shadow() const;
        void setShadow(bool renderShadow);
        int cullingMask() const;
        void setCullingMask(int cullingMask);
        TextureRenderTarget *target() const;
        void setTarget(TextureRenderTarget *target);

        void pick(glm::ivec2 point, std::function<void(GameObject*,int)> onPicked, glm::ivec2 size = glm::ivec2{1,1});

        std::shared_ptr<Material> const &replacementMaterial() const;
        void setReplacementMaterial(std::shared_ptr<Material> const &replacementMaterial);

        glm::vec2 const &viewportOffset() const;
        void setViewportOffset(glm::vec2 const &normalizedViewportOffset);
        glm::vec2 const &viewportDim() const;
        void setViewportDim(glm::vec2 const &normalizedViewportDim);
        virtual Ray screenPointToRay(glm::vec2 point) = 0;
    protected:
        glm::mat4 mProjectionMatrix;
        glm::vec2 mNormalizedViewportOffset = glm::vec2(0,0);
        glm::vec2 mNormalizedViewportDim = glm::vec2(1,1);
    private:
        void initShadowMap();
        void destroyShadowMap();
        void renderShadowMap(Light* directionalLight);
        void rebuildComponentList();
        void handleObjectPicking(EngineUniforms *engineUniforms);
        ComponentRenderable *includeComponent(Component* comp);
        TextureRenderTarget*mPickingRenderTarget = nullptr;
        std::shared_ptr<Texture2D> mPickingTexture;
        std::shared_ptr<Material> mPickingMaterial;
        std::shared_ptr<Shader> mShadowMapShader;
        std::shared_ptr<Material> mReplacementMaterial;
        Material*mShadowMapMaterial;
        EventListener<std::pair<Component*, ComponentUpdateStatus>> componentListener;
        void setupViewport(glm::vec2 &offset, glm::vec2 &dim);
        std::vector<ComponentRenderable*> mRenderableComponents;
        glm::vec4 mClearColor = glm::vec4(0,0,0,1);
        int mCullingMask = 0xffffffff;
        int mClearFlag; // default clear color clear depth
        bool mShadow = false;
        TextureRenderTarget*mTarget = nullptr;
        std::vector<PickEntry> mPickQueue;

    };
}