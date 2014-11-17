//
//  aabb.cpp
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 07/12/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/math/aabb.h"

using namespace glm;

namespace kick {
    const AABB AABB::mUninitialized{};
    
    AABB::AABB(vec3 min, vec3 max)
    :min(min), max(max){
    }
    
    AABB AABB::transform(mat4 mat)  const{
        glm::vec4 point(1,1,1,1);
        AABB res;
        float aabbIn[6] = {min.x, min.y, min.z, max.x, max.y, max.z};
        
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    point.x = aabbIn[i * 3];
                    point.y = aabbIn[j * 3 + 1];
                    point.z = aabbIn[k * 3 + 2];

                    res.addPoint(vec3(mat * point));
                }
            }
        }
        return res;
    }
    
    void AABB::addPoint(glm::vec3 point){
        min = glm::min(min, point);
        max = glm::max(max, point);
    }
    
    vec3 AABB::center() const{
        return (min + max) * 0.5f;
    }
    
    vec3 AABB::diagonal() const{
        return max-min;
    }
    
    bool AABB::operator==(const AABB &other) const {
        return min == other.min && max == other.max;
    }
    
    bool AABB::operator!=(const AABB &other) const {
        return min != other.min || max != other.max;
    }
    
    void AABB::operator=(const AABB &other){
        min = other.min;
        max = other.max;
    }
    
    bool AABB::uninitialized(){
        return *this == mUninitialized;
    }
    
    bool AABB::empty(){
        return min == max;
    }
    
    void AABB::clear(){
        *this = mUninitialized;
    }


    bool AABB::intersect(const Ray &r, float &tNear, float &tFar, int &nearAxis, int &farAxis){
        // http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm
        tNear = -std::numeric_limits<float>::infinity( ) ;
        tFar = std::numeric_limits<float>::infinity( ) ;

        for (int i=0;i<3;i++){
            if (r.direction()[i]==0){
                // the ray is parallel to the X planes
                // origin Xo is not between the slabs ( Xo < Xl or Xo > Xh) then return false
                if (r.origin()[i] < min[i] || r.origin()[i] > max[i]){
                    return false;
                }
            } else {
                float reciprocal = 1.0f / r.direction()[i];
                // if the ray is not parallel to the plane then
                // compute the intersection distance of the planes
                float t1 = (min[i]-r.origin()[i])*reciprocal;
                float t2 = (max[i]-r.origin()[i])*reciprocal;
                if (t1 > t2){
                    // swap since T1 intersection with near plane
                    float tmp = t1;
                    t1 = t2;
                    t2 = tmp;
                }
                if (t1 > tNear){
                    tNear = t1; /* want largest Tnear */
                    nearAxis = i;
                }
                if (t2 < tFar){
                    tFar = t2; /* want largest Tnear */
                    farAxis = i;
                }
                if (tNear > tFar){
                    return false; /*  box is missed */
                }
                if (tFar < 0){
                    return false; /* box is behind ray return false end */
                }
            }
        }
        return true;

    }

    bool AABB::contains(glm::vec3 point) {
        for (int i=0;i<3;i++){
            if (min[i]>point[i] || point[i] > max[i]){
                return false;
            }
        }
        return true;
    }

    bool AABB::intersect(const AABB &box) {
        for (int i=0;i<3;i++){
            if (min[i]>box.max[i] || max[i]<box.min[i]){
                return false;
            }
        }
        return true;
    }
}