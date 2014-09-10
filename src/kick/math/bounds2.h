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


        Bounds2() {
        }

        Bounds2(glm::vec2 const &min, glm::vec2 const &max)
                : min(min), max(max) {
        }

        glm::vec2 dimension(){ return max-min; }
        glm::vec2 center(){ return (max+min)*0.5f; }

        void expand(glm::vec2 p){
            min = glm::min(min, p);
            max = glm::max(max, p);
        }

        void expand(Bounds2 b){
            min = glm::min(min, b.min);
            max = glm::max(max, b.max);
        }

        void reset(){
            min = glm::vec2{ std::numeric_limits<float>::max() };
            max = glm::vec2{ std::numeric_limits<float>::lowest() };
        }

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

        Bounds2 lerp(float f, Bounds2 b){
            return Bounds2 {glm::mix(min, b.min, f), glm::mix(max, b.max, f)};

        }

        bool contains(glm::vec2 point){
            return min.x <= point.x && min.y <= point.y &&
                   max.x >= point.x && max.y >= point.y;
        }
    };

    inline std::ostream& operator<<(std::ostream& out, const Bounds2 & f){
        return out << "bounds{"<<glm::to_string(f.min) << ',' << glm::to_string(f.max) <<"}";
    }
}


