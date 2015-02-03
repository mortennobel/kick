//
//  math_test.h
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 18/12/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once

#include <iostream>


#include "math_test.h"
#include "tinytest/tinytest.h"
#include "kick/math/ray.h"
#include "kick/math/misc.h"
#include <cmath>
#include <iostream>

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtx/string_cast.hpp"
#include "kick/math/plane.h"
#include <cstdint>

using namespace std;
using namespace glm;

int RayClosestPointsTest(){
    vec3 pos1;
    vec3 pos2;
    kick::Ray ray1{vec3{0,2,-1},vec3{1,1,2}};
    kick::Ray ray2{vec3{1,0,-1},vec3{1,1,3}};
    bool found = ray1.closestPoints(ray2, pos1, pos2);
    TINYTEST_ASSERT(found);
    float distance = length(pos1 - pos2);
    TINYTEST_ASSERT(fabs(distance - (3.0f/2) * std::sqrt(2.0f)) < 0.01);
    return 1;
}

int RayClosestPointTest() {
    kick::Ray ray1{vec3{0,0,0},vec3{1,1,1}};
    kick::Ray ray2{vec3{0,0,0},vec3{-1,-1,-1}};
    vec3 point{1,0,0};
    vec3 cp1 = ray1.closestPoint(point);
    vec3 cp2 = ray2.closestPoint(point);
    TINYTEST_ASSERT(cp1 == cp2);

    kick::Ray ray3{vec3{0,5,0},vec3{1,0,0}};
    vec3 point3{5,0,0};
    vec3 expected{5,5,0};
    vec3 cp3 = ray3.closestPoint(point3);
    TINYTEST_ASSERT(expected == cp3);
    return 1;
}

int PlaneRayTest() {
    Plane plane(vec3{0.0,1.0,0.0},-1.0f);
    Ray ray(vec3(0,0,0), vec3(1,1,1));
    vec3 intersectionPoint = plane.intersectionPoint(ray);

    float dist1 = plane.distanceToPlane(vec3(1,1,1));
    TINYTEST_ASSERT(dist1 < 0.001);

    TINYTEST_ASSERT(length(intersectionPoint - vec3(1,1,1)) < 0.001);
    Ray ray2(vec3(1,0,1), vec3(1,1,1));
    vec3 intersectionPoint2 = plane.intersectionPoint(ray2);
    TINYTEST_ASSERT(length(intersectionPoint2 - vec3(2,1,2)) < 0.001);

    Plane plane3(vec3{0.0,1.0,0.0},1.0f);
    vec3 intersectionPoint3 = plane3.intersectionPoint(ray);
    TINYTEST_ASSERT(length(intersectionPoint3 - vec3(-1,-1,-1))<0.001);
    float dist3 = plane3.distanceToPlane(vec3(-1,-1,-1));
    TINYTEST_ASSERT(dist3< 0.001);
    return 1;
}

int TestIntPacking(){
    TINYTEST_ASSERT(INT32_MIN == vec4ToUint32(uint32ToVec4(INT32_MIN)));
    TINYTEST_ASSERT(INT32_MAX == vec4ToUint32(uint32ToVec4(INT32_MAX)));
    for (int64_t i = INT32_MIN ; i<= INT32_MAX ; i+=12345){ // some random samples
        int32_t res  = vec4ToUint32(uint32ToVec4((int32_t)i));
        for (int j=0;j<4;j++){
            auto v = uint32ToVec4((int32_t)i);
            TINYTEST_ASSERT(v[j] <= 1);
            TINYTEST_ASSERT(v[j] >= 0);
        }
        TINYTEST_ASSERT(res == i);
    }
    return 1;
}