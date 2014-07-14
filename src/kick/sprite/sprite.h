//
// Created by morten on 30/06/14.
//


#include "kick/scene/component.h"
#include "kick/scene/game_object.h"

#include "glm/glm.hpp"
#pragma once

namespace kick {
    class Sprite : public Component{
    public:
        Sprite(GameObject *gameObject) : Component(gameObject) {
        }

    private:

    };
}



