//
// Created by morten on 10/06/14.
//

#pragma once

#include "component.h"
#include "game_object.h"


namespace kick {
    // "Mixin" must be added to a Component to enable update events
    // Example:
    // class RotatorComponent : public Component, public Updatable {
    // //...
    // }
    class Updatable {
    public:
        virtual void update() = 0;
    };
}


