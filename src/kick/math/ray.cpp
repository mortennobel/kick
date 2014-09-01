//
//  ray.cpp
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 18/12/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/math/ray.h"

using namespace glm;

namespace kick {
    Ray::Ray()
    :origin{vec3(0,0,0)},direction{vec3(0,0,-1)}
    {
    
    }
    
    Ray::Ray(vec3 origin, vec3 direction)
    :origin{origin},direction{glm::normalize(direction)}
    {
    }
    
    bool Ray::closestPoints(Ray otherRay, glm::vec3& outPoint1, glm::vec3& outPoint2){
        vec3 otherRayOrigin = otherRay.origin;
        vec3 otherRayDirection = otherRay.direction;
        float epsilon = 1.401298E-45f;
        bool isParallel = fabs(dot(direction, otherRayDirection)) >= 1 - epsilon;
        if (isParallel){
            outPoint1 = vec3{0};
            outPoint2 = vec3{0};
            return false;
        }

        // loosely based on http://www.youtube.com/watch?v=HC5YikQxwZA
        // t is distance traveled at ray and s distance traveled at otherRay
        // PQ is vector between rays
        vec3 PQT = -direction;
        vec3 PQS = otherRayDirection;
        vec3 PQVal = -origin + otherRayOrigin;

        float PQu1S = dot(PQS, direction);
        float PQu1T = dot(PQT, direction);
        float PQu1Val = dot(PQVal, direction) * -1;

        float PQu2S = dot(PQS, otherRayDirection);
        float PQu2T = dot(PQT, otherRayDirection);
        float PQu2Val = dot(PQVal, otherRayDirection) * -1;

        // maybe not the most efficient way to solve the system of linear equations
        mat2x2 mat;
        mat[0][0] = PQu1S;
        mat[1][0] = PQu1T;
        mat[0][1] = PQu2S;
        mat[1][1] = PQu2T;

        mat = inverse(mat);
        vec2 res = mat * vec2(PQu1Val, PQu2Val);

        outPoint1 = origin + direction * res[1];
        outPoint2 = otherRayOrigin + otherRayDirection * res[0];

        return true;
    }
    
    void Ray::setDirection(glm::vec3 const &direction) {
        Ray::direction = glm::normalize(direction);
    }

    glm::vec3 const &Ray::getDirection() const {
        return direction;
    }

    void Ray::setOrigin(glm::vec3 const &origin) {
        Ray::origin = origin;
    }

    glm::vec3 const &Ray::getOrigin() const {
        return origin;
    }

    glm::vec3 Ray::getPoint(float offset) const {
        return origin + direction * offset;
    }
}