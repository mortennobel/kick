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
#include <utility>

namespace kick {
    class GameObject;
    
    enum class CameraType {
        Perspective = 0,
        Orthographic = 1
    };
    
    class Camera : public Component {
    public:
        Camera(GameObject *gameObject);
        virtual void activated();
        virtual void deactivated();
        virtual void render(EngineUniforms *engineUniforms);
        glm::vec4 getClearColor();
        void setClearColor(glm::vec4 clearColor);
        void setClearColor(bool clear);
        bool isClearColor();
        void setClearDepth(bool clear);
        bool isClearDepth();
        void setClearStencil(bool clear);
        bool isClearStencil();
        void setupCamera(EngineUniforms *engineUniforms);
        void setCameraType(CameraType cameraType);
        CameraType getCameraType();
        void setNear(float near);
        float getNear();
        void setFar(float far);
        float getFar();
        /// set field of view in degrees
        void setFieldOfView(float fov);
        /// set field of view in degrees
        float getFieldOfView();
        void setLeft(float left);
        float getLeft();
        void setRight(float right);
        float getRight();
        void setBottom(float bottom);
        float getBottom();
        void setTop(float top);
        float getTop();
    private:
        void rebuildComponentList();
        bool includeComponent(Component* comp);
        EventListener<std::pair<Component*, ComponentUpdateStatus>> componentListener;
        void setupViewport(glm::vec2 &offset, glm::vec2 &dim);
        std::vector<Component*> renderableComponents;
        glm::vec4 clearColor = glm::vec4(0,0,0,1);
        int clearFlag; // default clear color clear depth
        CameraType cameraType = CameraType::Perspective;
        void* renderTarget = nullptr;
        glm::vec2 normalizedViewportOffset = glm::vec2(0,0);
        glm::vec2 normalizedViewportDim = glm::vec2(1,1);
        union {
            struct {
                float near;
                float far;
                float fieldOfView;
            } perspective;
            struct {
                float near;
                float far;
                float left;
                float right;
                float bottom;
                float top;
            } orthographic;
        } cameraValues;
    };
}