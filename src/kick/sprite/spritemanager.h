//
// Created by morten on 30/06/14.
//

#pragma once

#include "kick/scene/component_renderable.h"
#include "kick/scene/game_object.h"

namespace kick {
class SpriteManager : public ComponentRenderable {
public:
    SpriteManager(GameObject *gameObject) : ComponentRenderable(gameObject) {
    }

    virtual void render(EngineUniforms *engineUniforms) override;
};
}



