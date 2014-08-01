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


    glm::vec3 Plane::getNormal() const {
        return (glm::vec3)plane;
    }

    void Plane::setNormal(glm::vec3 p) {
        for (int i=0;i<3;i++){
            plane[i] = p[i];
        }
    }

    float Plane::getDistanceToOrigo() const {
        return plane.z;
    }

    void Plane::setDistanceToOrigo(float d) {
        plane.z = d;
    }

    glm::vec3 Plane::getPointOnPlane() const {
        return getNormal() * getDistanceToOrigo();
    }

    float Plane::distanceToPlane(glm::vec3 point) const {
        return glm::dot(getNormal(), point) - getDistanceToOrigo();
    }

    float Plane::intersection(const Ray& line) const {
        using namespace std;
        glm::vec3 v0 = getPointOnPlane();
        glm::vec3 n = getNormal();
        glm::vec3 u = line.getPoint(1) - line.getOrigin();
        const double epsilon = 1e-10;
        double uDotN = glm::dot(u, n);
        if (abs(uDotN) < epsilon){
            return NAN;
        }

        return glm::dot(n, (v0-line.getOrigin())) / uDotN;
    }

    glm::vec3 Plane::intersectionPoint(const Ray& line) const {
        float res = intersection(line);
        return line.getPoint(res);
    }
}