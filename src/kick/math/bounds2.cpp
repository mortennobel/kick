//
// Created by morten on 05/07/14.
//

#include "bounds2.h"


namespace  kick{
Bounds2::Bounds2() {
}

Bounds2::Bounds2(glm::vec2 const &min, glm::vec2 const &max) : min(min), max(max) {
        }

glm::vec2 Bounds2::dimension() { return max-min; }

glm::vec2 Bounds2::center() { return (max+min)*0.5f; }

void Bounds2::expand(glm::vec2 p) {
    min = glm::min(min, p);
    max = glm::max(max, p);
}

void Bounds2::expand(Bounds2 b) {
    min = glm::min(min, b.min);
    max = glm::max(max, b.max);
}

void Bounds2::reset() {
    min = glm::vec2{ std::numeric_limits<float>::max() };
    max = glm::vec2{ std::numeric_limits<float>::lowest() };
}

glm::vec2 Bounds2::lowLeft() {
    return min;
}

glm::vec2 Bounds2::upperLeft() {
    return {min.x, max.y};
}

glm::vec2 Bounds2::lowRight() {
    return {max.x, min.y};
}

glm::vec2 Bounds2::upperRight() {
    return {max.x, max.y};
}

Bounds2 Bounds2::lerp(float f, Bounds2 b) {
    return Bounds2 {glm::mix(min, b.min, f), glm::mix(max, b.max, f)};

}

bool Bounds2::contains(glm::vec2 point) {
    return min.x <= point.x && min.y <= point.y &&
            max.x >= point.x && max.y >= point.y;
}

glm::vec2 Bounds2::diagonal() {
    return max-min;
}
}