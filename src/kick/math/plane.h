//
// Created by Morten Nobel-Jørgensen on 01/08/14.
//


#pragma once
#include "glm/glm.hpp"
#include "kick/math/ray.h"

namespace kick {
    // represent a plane using the equation ax + by + cz + d = 0, where (a,b,c) is the normal and d is negative distance
    // to plane from origin
    class Plane {
    public:
        Plane();

        Plane(float x, float y, float z, float offset);

        Plane(glm::vec3 normal, glm::vec3 pointInPlane);

        Plane(glm::vec3 normal, float offset);

        glm::vec3 normal() const;

        void setNormal(glm::vec3 p);

        float offset() const;

        void setOffset(float d);

        glm::vec3 pointOnPlane() const;

        float distanceToPlane(glm::vec3 point) const;

        float intersection(const Ray& line) const;

        glm::vec3 intersectionPoint(const Ray& line) const;
    private:
        glm::vec4 plane;
    };
}


