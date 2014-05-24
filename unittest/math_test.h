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
#include <cmath>
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtx/string_cast.hpp"

using namespace std;
using namespace glm;

int RayClosestPointsTest(){
    vec3 pos1;
    vec3 pos2;
    bool found = kick::Ray::ClosestPoints(vec3{0,2,-1},vec3{1,1,2},vec3{1,0,-1},vec3{1,1,3},pos1, pos2);
    TINYTEST_ASSERT(found);
    float distance = length(pos1 - pos2);
    TINYTEST_ASSERT(fabs(distance - (3.0f/2) * sqrt(2.0f)) < 0.01);
    return 1;
}

int FrustumAABBTest(){

    return 1;
}

int ProjectionTest(){

    mat4 res = perspective(45.0f,
            1.0f,
            0.1f,
            100.0f);
    vec4 one{1,1,1,1};
    vec4 resVector = res*one;
    cout << to_string(resVector) << endl;
    return 1;
}
