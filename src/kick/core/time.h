//
// Created by morten on 18/05/14.
//

#pragma once

#include <iostream>

namespace kick {
    class Engine;

    class Time {
    public:
        // return time since startup
        static float getTime();
        // return time since startup
        static float getDeltaTime();
        // return frame count since startup
        static long getFrame();
        friend class Engine;
    private:
        static long frame;
        static float deltaTime;
    };
}