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
        glm::vec3 dimension(){ return max-min; }
        glm::vec3 center(){ return (max+min)*0.5f; }

        Bounds3() {
        }

        Bounds3(glm::vec3 const &min, glm::vec3 const &max)
        : min(min), max(max) {
        }

        void expand(glm::vec3 p){
            min = glm::min(min, p);
            max = glm::max(max, p);
        }

        void expand(Bounds3 b){
            min = glm::min(min, b.min);
            max = glm::max(max, b.max);
        }

        void reset(){
            min = glm::vec3{ std::numeric_limits<float>::max() };
            max = glm::vec3{ std::numeric_limits<float>::lowest() };
        }

        bool contains(glm::vec3 point){
            return min.x <= point.x && min.y <= point.y && min.z <= point.z &&
                    max.x >= point.x && max.y >= point.y && max.z >= point.z
                    ;
        }
    };

    inline std::ostream& operator<<(std::ostream& out, const Bounds3 & f){
        return out << "bounds{"<<glm::to_string(f.min) << ',' << glm::to_string(f.max) <<"}";
    }
}



