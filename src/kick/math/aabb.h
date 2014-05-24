//
//  aabb.h
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 07/12/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include <limits>

namespace kick {
    struct AABB {
        AABB(glm::vec3 min = glm::vec3(FLT_MAX), glm::vec3 max = glm::vec3(-FLT_MAX));
        
        /**
         * Transforms the eight points of the Axis-Aligned Bounding Box into a new AABB
         */
        AABB transform(glm::mat4 mat) const;
        
        /**
         * Set value to {max,min}
         */
        void clear();
        
        void addPoint(glm::vec3 point);
        glm::vec3 center() const;
        glm::vec3 diagonal() const;
        glm::vec3 min;
        glm::vec3 max;
        
        bool operator==(const AABB &other) const;
        bool operator!=(const AABB &other) const;
        void operator=(const AABB &other);
        
        /**
         * Returns true if min = vec3(FLT_MAX) and max = vec3(-FLT_MAX)
         */
        bool IsUninitialized();
        
        /**
         * Returns true if min == max
         */
        bool IsEmpty();
        
        static const AABB uninitialized;
    };
};

