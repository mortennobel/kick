//
//  frustum.cpp
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 21/12/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/math/frustum.h"
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

namespace kick {
    FrustumIntersection Frustum::intersectAabb(Bounds3 aabb){
        vec3 center = aabb.center();
        vec3 halfVector = aabb.diagonal() * 0.5f;
        FrustumIntersection result = FrustumIntersection::Inside;
        
        for (int i = 0; i < 6; i++) {
            vec3 planeNormal = vec3(planes[i]);
            float e = dot(halfVector, abs(planeNormal));
            float s = dot(center, planeNormal) + planes[i].w;
            
            if (s - e > 0) {
                // inside
            } else if (s + e < 0) { // outside
                return FrustumIntersection::Outside;
            } else  {  // intersecting
                result = FrustumIntersection::Intersecting;
            }
        }
        return result;
    }

    void Frustum::extractPlanes(glm::mat4 viewProjectionMatrix, bool normalizePlaneNormals){
        float* _viewProjectionMatrix = value_ptr(viewProjectionMatrix);
        auto _11 = _viewProjectionMatrix[0], _21 = _viewProjectionMatrix[1], _31 = _viewProjectionMatrix[2], _41 = _viewProjectionMatrix[3],
        _12 = _viewProjectionMatrix[4], _22 = _viewProjectionMatrix[5], _32 = _viewProjectionMatrix[6], _42 = _viewProjectionMatrix[7],
        _13 = _viewProjectionMatrix[8], _23 = _viewProjectionMatrix[9], _33 = _viewProjectionMatrix[10], _43 = _viewProjectionMatrix[11],
        _14 = _viewProjectionMatrix[12], _24 = _viewProjectionMatrix[13], _34 = _viewProjectionMatrix[14], _44 = _viewProjectionMatrix[15];
        
        // Left clipping plane
        planes[0] = vec4{
            _41 + _11,
            _42 + _12,
            _43 + _13,
            _44 + _14};
        
        // Right clipping plane
        planes[1] = vec4{
            _41 - _11,
            _42 - _12,
            _43 - _13,
            _44 - _14
        };
        // Top clipping plane
        planes[2] = vec4{
            _41 - _21,
            _42 - _22,
            _43 - _23,
            _44 - _24};
        // Bottom clipping plane
        planes[3] = vec4{
            _41 + _21,
            _42 + _22,
            _43 + _23,
            _44 + _24
        };
        // Near clipping plane
        planes[4] = vec4{
            _41 + _31,
            _42 + _32,
            _43 + _33,
            _44 + _34
        };
        // Far clipping plane
        planes[5] = vec4{
            _41 - _31,
            _42 - _32,
            _43 - _33,
            _44 - _34
        };
        if (normalizePlaneNormals) {
            for (int i=0;i<6;i++){
                planes[i] = normalize(planes[i]);
            }
        }
    }
}
