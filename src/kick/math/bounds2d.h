//
// Created by morten on 05/07/14.
//


#pragma once

#include "glm/glm.hpp"
#include <iostream>
#include "glm/gtx/string_cast.hpp"

namespace kick {
    struct Bounds2D {
    public:
        glm::vec2 min;
        glm::vec2 max;
        glm::vec2 dimension(){ return max-min; }
        glm::vec2 center(){ return (max+min)*0.5f; }

        glm::vec2 lowLeft(){
            return min;
        }

        glm::vec2 upperLeft(){
            return {min.x, max.y};
        }

        glm::vec2 lowRight(){
            return {max.x, min.y};
        }

        glm::vec2 upperRight(){
            return {max.x, max.y};
        }

        bool contains(glm::vec2 point){
            return min.x <= point.x && min.y <= point.y &&
                max.x >= point.x && max.y >= point.y;
        }
    };

    inline std::ostream& operator<<(std::ostream& out, const Bounds2D& f){
        return out << "bounds{"<<glm::to_string(f.min) << ',' << glm::to_string(f.max) <<"}";
    }
}


