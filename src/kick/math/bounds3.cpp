//
// Created by Morten Nobel-Jørgensen on 08/09/14.
//

#include "bounds3.h"

namespace kick{

Bounds3::Bounds3() {
}

Bounds3::Bounds3(glm::vec3 const &min, glm::vec3 const &max) : min(min), max(max) {
        }

void Bounds3::expand(glm::vec3 p) {
    min = glm::min(min, p);
    max = glm::max(max, p);
}

void Bounds3::expand(Bounds3 b) {
    min = glm::min(min, b.min);
    max = glm::max(max, b.max);
}

void Bounds3::reset() {
    min = glm::vec3{ std::numeric_limits<float>::max() };
    max = glm::vec3{ std::numeric_limits<float>::lowest() };
}

Bounds3 Bounds3::lerp(float f, Bounds3 b) {
    return Bounds3 {glm::mix(min, b.min, f), glm::mix(max, b.max, f)};

}

bool Bounds3::contains(glm::vec3 point) {
    return min.x <= point.x && min.y <= point.y && min.z <= point.z &&
            max.x >= point.x && max.y >= point.y && max.z >= point.z
            ;
}
}