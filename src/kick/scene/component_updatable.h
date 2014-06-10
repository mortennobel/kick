//
// Created by morten on 10/06/14.
//

#pragma once

#include "component.h"
#include "game_object.h"


namespace kick {
    class ComponentUpdateable : public Component {

    public:
        ComponentUpdateable(GameObject *gameObject)
                : Component(gameObject) {
        }

        virtual void update() = 0;
    };
}


