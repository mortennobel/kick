//
// Created by morten on 05/07/14.
//


#pragma once

#include "glm/glm.hpp"
#include <iostream>
#include "glm/gtx/string_cast.hpp"

namespace kick {
    struct Bounds2 {
    public:
        glm::vec2 min = glm::vec2{ std::numeric_limits<float>::max() };
        glm::vec2 max = glm::vec2{ std::numeric_limits<float>::lowest() };

        Bounds2();

        Bounds2(glm::vec2 const &min, glm::vec2 const &max);

        glm::vec2 dimension();

        glm::vec2 center();

        glm::vec2 diagonal();

        void expand(glm::vec2 p);

        void expand(Bounds2 b);

        void reset();

        glm::vec2 lowLeft();

        glm::vec2 upperLeft();

        glm::vec2 lowRight();

        glm::vec2 upperRight();

        Bounds2 lerp(float f, Bounds2 b);

        bool contains(glm::vec2 point);
    };

    inline std::ostream& operator<<(std::ostream& out, const Bounds2 & f){
        return out << "bounds{"<<glm::to_string(f.min) << ',' << glm::to_string(f.max) <<"}";
    }
}


