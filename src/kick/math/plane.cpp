//
// Created by Morten Nobel-Jørgensen on 01/08/14.
//

#include "plane.h"

namespace kick {

    Plane::Plane() : plane{1,0,0,0} {
    }

    Plane::Plane(float x, float y, float z, float dist) : plane{x,y,z,dist} {
    }

    Plane::Plane(glm::vec3 direction, float dist) : plane{direction,dist} {
    }


    glm::vec3 Plane::normal() const {
        return (glm::vec3)plane;
    }

    void Plane::setNormal(glm::vec3 p) {
        for (int i=0;i<3;i++){
            plane[i] = p[i];
        }
    }

    float Plane::distanceToOrigo() const {
        return plane.z;
    }

    void Plane::setDistanceToOrigo(float d) {
        plane.z = d;
    }

    glm::vec3 Plane::pointOnPlane() const {
        return normal() * distanceToOrigo();
    }

    float Plane::distanceToPlane(glm::vec3 point) const {
        return glm::dot(normal(), point) - distanceToOrigo();
    }

    float Plane::intersection(const Ray& line) const {
        using namespace std;
        glm::vec3 v0 = pointOnPlane();
        glm::vec3 n = normal();
        glm::vec3 u = line.point(1) - line.origin();
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