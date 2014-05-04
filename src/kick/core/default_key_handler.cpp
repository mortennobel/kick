//
// Created by morten on 04/05/14.
//

#include "kick/core/default_key_handler.h"
#include "kick/core/engine.h"
#include "kick/context/context.h"
#include <iostream>
#include <vector>

using namespace std;

kick::DefaultKeyHandler::DefaultKeyHandler()
:fullScreen{{KeyMapping{Key::LGUI, Key::f}, KeyMapping{Key::RGUI, Key::f}}}
{
}

void kick::DefaultKeyHandler::handleKeyPress(kick::Engine *engine, kick::KeyInput& input) {
    if (input.down(fullScreen)){
        auto context = engine->getContext();
        context->setFullscreen(!context->isFullscreen());
    }
}
