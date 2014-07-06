//
// Created by morten on 05/07/14.
//


#pragma once

#include "glm/glm.hpp"

namespace kick {
    struct Bounds2D {
    public:
        glm::vec2 min;
        glm::vec2 max;
        glm::vec2 dimension(){ return max-min; }
        glm::vec2 center(){ return (max+min)*0.5f; }
    };
}


