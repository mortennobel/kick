//
// Created by morten on 10/06/14.
//


#pragma once

#include "kick/scene/component.h"
#include "game_object.h"

namespace kick {
    class EngineUniforms;

    class ComponentRenderable : public Component{
    public:
        ComponentRenderable(GameObject *gameObject)
                : Component(gameObject) {
        }

        virtual void render(EngineUniforms *engineUniforms) = 0;
    };
}


