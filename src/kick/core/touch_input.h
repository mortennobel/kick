//
// Created by morten on 07/08/14.
//


#pragma once

#include <map>
#include "glm/glm.hpp"

namespace kick {

    struct Touch{
        bool down;
        bool up;
        int64_t fingerId;
        glm::ivec2 pos;
        glm::ivec2 delta;
        float pressure;     /**< Normalized in the range 0...1 */
    };

    class TouchInput {
    public:
        static void setTouchStarted(int64_t fingerid, glm::ivec2 pos, float pressure);
        static void setTouchMoved(int64_t fingerid, glm::ivec2 pos, float pressure);
        static void setTouchEnded(int64_t fingerid, glm::ivec2 pos);
        static void reset();
        static std::map<int64_t,Touch>::const_iterator begin();
        static std::map<int64_t,Touch>::const_iterator end();
    private:
        TouchInput() = delete;
        static std::map<int64_t,Touch> touches;
    };
}



