//
//  ray.h
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 18/12/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "glm/glm.hpp"

namespace kick {
    struct Ray {
        Ray();
        Ray(glm::vec3 origin, glm::vec3 direction);
        
        bool ClosestPoints(Ray otherRay, glm::vec3& outPoint1, glm::vec3& outPoint2);
        
        static bool ClosestPoints(glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::vec3 otherRayOrigin, glm::vec3 otherRayDirection, glm::vec3& outPoint1, glm::vec3& outPoint2);
        
        glm::vec3 origin;
        glm::vec3 direction;
    };
}