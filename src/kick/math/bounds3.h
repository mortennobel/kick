//
// Created by Morten Nobel-Jørgensen on 08/09/14.
//


#pragma once

#include "glm/glm.hpp"
#include <iostream>
#include "glm/gtx/string_cast.hpp"
#include <limits>

namespace kick {
    struct Bounds3 {
    public:
        glm::vec3 min = glm::vec3{ std::numeric_limits<float>::max() };
        glm::vec3 max = glm::vec3{ std::numeric_limits<float>::lowest() };

        glm::vec3 dimension();

        glm::vec3 center();

        glm::vec3 diagonal();

        Bounds3();

        Bounds3(glm::vec3 const &min, glm::vec3 const &max);

        void expand(glm::vec3 p);

        void expand(Bounds3 b);

        void reset();

        Bounds3 lerp(float f, Bounds3 b);

        bool contains(glm::vec3 point);
    };

    inline std::ostream& operator<<(std::ostream& out, const Bounds3 & f){
        return out << "bounds{"<<glm::to_string(f.min) << ',' << glm::to_string(f.max) <<"}";
    }
}



