//
//  ray.h
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 18/12/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once

#include "glm/glm.hpp"

namespace kick {
    class Ray {
    public:
        Ray();
        Ray(glm::vec3 origin, glm::vec3 direction);
        
        bool closestPoints(Ray otherRay, glm::vec3& outPoint1, glm::vec3& outPoint2);
        
        glm::vec3 const &origin() const;
        void setOrigin(glm::vec3 const &origin);
        glm::vec3 const &direction() const;
        void setDirection(glm::vec3 const &direction);

        glm::vec3 point(float offset) const;
    private:
        glm::vec3 mOrigin;
        glm::vec3 mDirection;
    };
}