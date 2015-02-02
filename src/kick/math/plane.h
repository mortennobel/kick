//
// Created by Morten Nobel-Jørgensen on 01/08/14.
//


#pragma once
#include "glm/glm.hpp"
#include "kick/math/ray.h"

namespace kick {
    class Plane {
    public:
        Plane();

        Plane(float x, float y, float z, float dist);

        Plane(glm::vec3 normal, glm::vec3 pointInPlane);

        Plane(glm::vec3 normal, float dist);

        glm::vec3 normal() const;

        void setNormal(glm::vec3 p);

        float distanceToOrigo() const;

        void setDistanceToOrigo(float d);

        glm::vec3 pointOnPlane() const;

        float distanceToPlane(glm::vec3 point) const;

        float intersection(const Ray& line) const;

        glm::vec3 intersectionPoint(const Ray& line) const;
    private:
        glm::vec4 plane;
    };
}


