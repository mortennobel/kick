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
        // returns true if mouse button was pressed down in this frame
        static bool down(int button);
        // returns true if mouse button is down is this frame
        static bool pressed(int button);
        // returns true if mouse button is released in this frame
        static bool up(int button);
        static void buttonPressStarted(int button);
        static void buttonPressEnded(int button);

        static glm::ivec2 position();

        static void setPosition(glm::ivec2 position);

        static glm::ivec2 positionDelta();

        static void setPositionDelta(glm::ivec2 positionDelta);

        static glm::ivec2 mouseWheelDelta();

        static void setMouseWheelDelta(glm::ivec2 mouseWheelDelta);

        /// clear the frame specific properties
        static void reset();
        static const int mouseButtons = 5;

        static int clicks();

        static void setClicks(int clicks);
    private:
        MouseInput() = delete;
        // absolute mouse position (relative to upper left corner)
        static glm::ivec2 mPosition;
        static glm::ivec2 mPositionDelta;
        static glm::ivec2 mMouseWheelDelta;
        static int mClicks;

        static bool mButtonDown[mouseButtons];
        static bool mButtonPressed[mouseButtons];
        static bool mButtonUp[mouseButtons];
    };
}
