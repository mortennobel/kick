//
// Created by morten on 19/09/14.
//


#pragma once

#include <cstdint>
#include "glm/glm.hpp"

namespace kick {
    // stores a int 32 in vec4
    glm::vec4 uint32ToVec4(int32_t uint32);

    // restores a int 32 from vec4
    int32_t vec4ToUint32(glm::vec4 vec);

    // restores a int 32 from vec4
    int32_t vec4ToUint32(glm::u8vec4 vec);

    float clamp01(float value);

    float repeat(float t, float length);

    float lerpAngle(float a, float b, float t);

    glm::vec3 faceNormal(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);
}

