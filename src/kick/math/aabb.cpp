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
    const AABB AABB::uninitialized{};
    
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
    
    bool AABB::IsUninitialized(){
        return *this == uninitialized;
    }
    
    bool AABB::IsEmpty(){
        return min == max;
    }
    
    void AABB::clear(){
        *this = uninitialized;
    }
    
    
}