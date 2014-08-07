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
        void setTouchStarted(int64_t fingerid, glm::ivec2 pos, float pressure);
        void setTouchMoved(int64_t fingerid, glm::ivec2 pos, float pressure);
        void setTouchEnded(int64_t fingerid, glm::ivec2 pos);
        void reset();
        std::map<int64_t,Touch>::const_iterator begin() const;
        std::map<int64_t,Touch>::const_iterator end() const;
    private:
        std::map<int64_t,Touch> touches;
    };
}



