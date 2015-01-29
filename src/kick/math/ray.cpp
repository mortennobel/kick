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
    : mOrigin{vec3(0,0,0)}, mDirection{vec3(0,0,-1)}
    {
    
    }
    
    Ray::Ray(vec3 origin, vec3 direction)
    : mOrigin{origin}, mDirection{glm::normalize(direction)}
    {
    }
    
    bool Ray::closestPoints(Ray otherRay, glm::vec3& outPoint1, glm::vec3& outPoint2) const {
        vec3 otherRayOrigin = otherRay.mOrigin;
        vec3 otherRayDirection = otherRay.mDirection;
        float epsilon = 1.401298E-45f;
        bool isParallel = fabs(dot(mDirection, otherRayDirection)) >= 1 - epsilon;
        if (isParallel){
            outPoint1 = vec3{0};
            outPoint2 = vec3{0};
            return false;
        }

        // loosely based on http://www.youtube.com/watch?v=HC5YikQxwZA
        // t is distance traveled at ray and s distance traveled at otherRay
        // PQ is vector between rays
        vec3 PQT = -mDirection;
        vec3 PQS = otherRayDirection;
        vec3 PQVal = -mOrigin + otherRayOrigin;

        float PQu1S = dot(PQS, mDirection);
        float PQu1T = dot(PQT, mDirection);
        float PQu1Val = dot(PQVal, mDirection) * -1;

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

        outPoint1 = mOrigin + mDirection * res[1];
        outPoint2 = otherRayOrigin + otherRayDirection * res[0];

        return true;
    }
    
    void Ray::setDirection(glm::vec3 const &direction) {
        Ray::mDirection = glm::normalize(direction);
    }

    glm::vec3 const &Ray::direction() const {
        return mDirection;
    }

    void Ray::setOrigin(glm::vec3 const &origin) {
        Ray::mOrigin = origin;
    }

    glm::vec3 const &Ray::origin() const {
        return mOrigin;
    }

    glm::vec3 Ray::point(float offset) const {
        return mOrigin + mDirection * offset;
    }

    bool Ray::intersectTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 &intersectionPoint, bool clampBackIntersections) const {
        // based on RTR 3ed page 750
        vec3 e1 = v1 - v0;
        vec3 e2 = v2 - v0;
        vec3 q = cross(mDirection, e2);
        float a = dot(e1, q);
        float eps = 1e-10;
        if (a > -eps && a < eps) return false;
        float f = 1 / a;
        vec3 s = mOrigin - v0;
        float u = f * dot(s, q);
        if (u < 0.0) return false;
        vec3 r = cross(s, e1);
        float v = f * dot(mDirection, r);
        if (v < 0.0 || u + v > 1.0) return false;
        float t = f * dot(e2, r);
        if (clampBackIntersections && t < 0){
            return false;
        }
        intersectionPoint = point(t);

        return true;
    }

    glm::vec3 Ray::closestPoint(glm::vec3 point) const {
        return mOrigin + dot(point - mOrigin, mDirection) / dot(mDirection,mDirection) * mDirection;
    }
}