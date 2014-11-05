//
//  frustum.h
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 21/12/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once

#include "glm/glm.hpp"
#include "kick/math/bounds3.h"

namespace kick {
    enum class FrustumIntersection {
        Outside,
        Inside,
        Intersecting
    };
    
    
    /// Frustum represented as 6 plane equations (a*x+b*y+c*z+d=0 , where [a,b,c] is the normal of the plane).
    /// Note the normals of the frustum points inwards. The order of the planes are left, right, top, bottom, near, far
    /// The implementation is based on
    /// "Fast Extraction of Viewing Frustum Planes from the WorldView-Projection Matrix" by Gil Grib and Klaus Hartmann
    /// http://www.cs.otago.ac.nz/postgrads/alexis/planeExtraction.pdf
    class Frustum {
    public:
        FrustumIntersection intersectAabb(Bounds3 bounds);
        void extractPlanes(glm::mat4 viewProjectionMatrix, bool normalizePlaneNormals = false);
    private:
        glm::vec4 planes[6];
    };
}
