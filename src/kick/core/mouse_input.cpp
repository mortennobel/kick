//
//  mouse_input.cpp
//  UnitTest_KickCPP
//
//  Created by morten on 24/04/14.
//
//

#include "mouse_input.h"
#include <cassert>

using namespace glm;

#define validButton(button) assert(button >=0); assert(button < mouseButtons);

namespace kick {
    MouseInput::MouseInput(){
        for (int i=0;i<mouseButtons;i++){
            buttonDown[i] = false;
            buttonPressed[i] = false;
            buttonUp[i] = false;
        }
    }
    
    void MouseInput::reset(){
        mouseWheelDelta = ivec2(0);
        for (int i=0;i<mouseButtons;i++){
            buttonDown[i] = false;
            buttonUp[i] = false;
        }
    }

    bool MouseInput::down(int button) const {
        validButton(button);
        return buttonDown[button];
    }

    bool MouseInput::pressed(int button) const {
        validButton(button);
        return buttonPressed[button];
    }

    bool MouseInput::up(int button) const {
        validButton(button);
        return buttonUp[button];
    }

    void MouseInput::buttonPressStarted(int button) {
        validButton(button);
        buttonDown[button] = true;
        buttonPressed[button] = true;
    }

    void MouseInput::buttonPressEnded(int button) {
        validButton(button);
        buttonUp[button] = true;
        buttonPressed[button] = false;
    }

    void MouseInput::setMouseWheelDelta(glm::ivec2 mouseWheelDelta) {
        MouseInput::mouseWheelDelta = mouseWheelDelta;
    }

    glm::ivec2 MouseInput::getMouseWheelDelta() const {
        return mouseWheelDelta;
    }

    void MouseInput::setPositionDelta(glm::ivec2 positionDelta) {
        MouseInput::positionDelta = positionDelta;
    }

    glm::ivec2 MouseInput::getPositionDelta() const {
        return positionDelta;
    }

    void MouseInput::setPosition(glm::ivec2 position) {
        MouseInput::position = position;
    }

    glm::ivec2 MouseInput::getPosition() const {
        return position;
    }
}