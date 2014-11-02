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

    glm::ivec2 MouseInput::mPosition{0};
    glm::ivec2 MouseInput::mPositionDelta{0};
    glm::ivec2 MouseInput::mMouseWheelDelta{0};
    int MouseInput::mClicks {0};

    bool MouseInput::mButtonDown[] = {false, false, false, false};
    bool MouseInput::mButtonPressed[] = {false, false, false, false};
    bool MouseInput::mButtonUp[] = {false, false, false, false};

    void MouseInput::reset(){
        mPositionDelta = ivec2(0);
        mMouseWheelDelta = ivec2(0);
        for (int i=0;i<mouseButtons;i++){
            mButtonDown[i] = false;
            mButtonUp[i] = false;
        }
    }

    bool MouseInput::down(int button) {
        validButton(button);
        return mButtonDown[button];
    }

    bool MouseInput::pressed(int button) {
        validButton(button);
        return mButtonPressed[button];
    }

    bool MouseInput::up(int button) {
        validButton(button);
        return mButtonUp[button];
    }

    void MouseInput::buttonPressStarted(int button) {
        validButton(button);
        mButtonDown[button] = true;
        mButtonPressed[button] = true;
    }

    void MouseInput::buttonPressEnded(int button) {
        validButton(button);
        mButtonUp[button] = true;
        mButtonPressed[button] = false;
    }

    void MouseInput::setMouseWheelDelta(glm::ivec2 mouseWheelDelta) {
        MouseInput::mMouseWheelDelta = mouseWheelDelta;
    }

    glm::ivec2 MouseInput::mouseWheelDelta() {
        return mMouseWheelDelta;
    }

    void MouseInput::setPositionDelta(glm::ivec2 positionDelta) {
        MouseInput::mPositionDelta = positionDelta;
    }

    glm::ivec2 MouseInput::positionDelta() {
        return mPositionDelta;
    }

    void MouseInput::setPosition(glm::ivec2 position) {
        MouseInput::mPosition = position;
    }

    glm::ivec2 MouseInput::position() {
        return mPosition;
    }

    int MouseInput::clicks() {
        return mClicks;
    }

    void MouseInput::setClicks(int clicks) {
        MouseInput::mClicks = clicks;
    }
}