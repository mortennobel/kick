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
#include "component_renderable.h"
#include <utility>

namespace kick {
    class GameObject;
    class Shader;
    class Material;
    class TextureRenderTarget;

    class Camera : public Component {
    public:
        Camera(GameObject *gameObject);
        ~Camera();
        virtual void activated();
        virtual void deactivated();
        virtual void render(EngineUniforms *engineUniforms);




// reset matrix if used parameters (if any)
        virtual void resetProjectionMatrix();
        glm::vec4 getClearColor();
        void setClearColor(glm::vec4 clearColor);
        void setClearColor(bool clear);
        bool isClearColor();
        void setClearDepth(bool clear);
        bool isClearDepth();
        void setClearStencil(bool clear);
        bool isClearStencil();
        void setupCamera(EngineUniforms *engineUniforms);
        glm::mat4 getProjectionMatrix();
        // override projection matrix
        void setProjectionMatrix(glm::mat4 projectionMatrix);
        bool isShadow() const;
        void setShadow(bool renderShadow);
        int getCullingMask() const;
        void setCullingMask(int cullingMask);
        TextureRenderTarget * getTarget() const;
        void setTarget(TextureRenderTarget *target);
    protected:
        glm::mat4 projectionMatrix;
        glm::vec2 normalizedViewportOffset = glm::vec2(0,0);
        glm::vec2 normalizedViewportDim = glm::vec2(1,1);
    private:
        void initShadowMap();
        void destroyShadowMap();
        void renderShadowMap(Light* directionalLight);
        void rebuildComponentList();
        ComponentRenderable *includeComponent(Component* comp);
        Shader* shadowMapShader;
        Material* shadowMapMaterial;
        EventListener<std::pair<Component*, ComponentUpdateStatus>> componentListener;
        void setupViewport(glm::vec2 &offset, glm::vec2 &dim);
        std::vector<ComponentRenderable*> renderableComponents;
        glm::vec4 clearColor = glm::vec4(0,0,0,1);
        int cullingMask = 0xffffffff;
        int clearFlag; // default clear color clear depth
        bool shadow = false;
        TextureRenderTarget* target = nullptr;
    };
}