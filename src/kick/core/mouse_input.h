//
//  mouse_input.h
//  UnitTest_KickCPP
//
//  Created by morten on 24/04/14.
//
//

#pragma once

#include <iostream>
#include <set>

#include "glm/glm.hpp"

namespace kick {
    class MouseInput {
    public:
        MouseInput();
        MouseInput(const MouseInput&) = delete;
        // returns true if mouse button was pressed down in this frame
        bool down(int button) const;
        // returns true if mouse button is down is this frame
        bool pressed(int button)  const;
        // returns true if mouse button is released in this frame
        bool up(int button) const;
        void buttonPressStarted(int button);
        void buttonPressEnded(int button);


        glm::ivec2 getPosition() const;

        void setPosition(glm::ivec2 position);

        glm::ivec2 getPositionDelta() const;

        void setPositionDelta(glm::ivec2 positionDelta);

        glm::ivec2 getMouseWheelDelta() const;

        void setMouseWheelDelta(glm::ivec2 mouseWheelDelta);

/// clear the frame specific properties
        void reset();
        static const int mouseButtons = 5;

        int getClicks() const;

        void setClicks(int clicks);

    private:
        // absolute mouse position (relative to upper left corner)
        glm::ivec2 position;
        glm::ivec2 positionDelta;
        glm::ivec2 mouseWheelDelta;
        int clicks = 0;

        bool buttonDown[mouseButtons];
        bool buttonPressed[mouseButtons];
        bool buttonUp[mouseButtons];
    };
}
