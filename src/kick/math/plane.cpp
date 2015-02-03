//
// Created by Morten Nobel-Jørgensen on 01/08/14.
//

#include "plane.h"
#include "ray.h"

namespace kick {

    Plane::Plane() : plane{1,0,0,0} {
    }

    Plane::Plane(glm::vec3 normal_, glm::vec3 pointInPlane)
    :plane(normal_, -dot(normal_, pointInPlane))
    {
    }

    Plane::Plane(float x, float y, float z, float offset) : plane{x,y,z, offset} {
    }

    Plane::Plane(glm::vec3 normal_, float offset)
            : plane{normal_, offset} {
    }


    glm::vec3 Plane::normal() const {
        return (glm::vec3)plane;
    }

    void Plane::setNormal(glm::vec3 p) {
        for (int i=0;i<3;i++){
            plane[i] = p[i];
        }
    }

    float Plane::offset() const {
        return plane.w;
    }

    void Plane::setOffset(float d) {
        plane.w = d;
    }

    glm::vec3 Plane::pointOnPlane() const {
        return normal() * -offset();
    }

    float Plane::distanceToPlane(glm::vec3 point) const {
        return glm::dot(normal(), point) + offset();
    }

    float Plane::intersection(const Ray& line) const {
        using namespace std;
        glm::vec3 v0 = pointOnPlane();
        glm::vec3 n = glm::normalize(normal());
        glm::vec3 u = glm::normalize(line.point(1) - line.origin());
        const double epsilon = 1e-10;
        double uDotN = glm::dot(u, n);
        if (abs(uDotN) < epsilon){
            return NAN;
        }

        return glm::dot(n, (v0- line.origin())) / uDotN;
    }

    glm::vec3 Plane::intersectionPoint(const Ray& line) const {
        float res = intersection(line);
        return line.point(res);
    }
}