//
// Created by morten on 10/06/14.
//


#pragma once

#include "kick/scene/component.h"
#include "game_object.h"
#include "glm/glm.hpp"

namespace kick {
    struct EngineUniforms;
    class Material;

    class ComponentRenderable : public Component{
    public:
        ComponentRenderable(GameObject *gameObject)
                : Component(gameObject) {
        }

        virtual void render(EngineUniforms *engineUniforms, Material* replacementMaterial = nullptr) = 0;

        // return the (shader) render order
        // 0-999: Background. Mainly for skyboxes etc
        // 1000-1999 Opaque geometry (default)
        // 2000-2999 Transparent. This queue is sorted in a back to front order before rendering.
        // 3000-3999 Overlay
        virtual int renderOrder() = 0;
    };
}


