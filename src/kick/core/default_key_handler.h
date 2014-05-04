//
// Created by morten on 04/05/14.
//


#pragma once

#include "kick/core/key_input.h"


namespace kick {
    class Engine;

    class DefaultKeyHandler {
    public:
        DefaultKeyHandler();
        KeyMapping fullScreen;

        void handleKeyPress(Engine *pEngine, KeyInput& input);
    };

}


