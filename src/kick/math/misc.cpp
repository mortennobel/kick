//
// Created by morten on 19/09/14.
//

#include "misc.h"
#include <glm/gtc/constants.hpp>

namespace kick {

    glm::vec4 uint32ToVec4(int32_t uint32) {
        uint8_t * ptr = (uint8_t *)(&uint32);
        glm::vec4 dest;
        for (int i = 0; i < 4; i++) {
            dest[i] = ptr[i]/255.0f;
        }
        return dest;
    }

    int32_t vec4ToUint32(glm::vec4 vec) {
        int32_t uint32;
        uint8_t * ptr = (uint8_t *)(&uint32);
        for (int i = 0; i < 4; i++) {
            ptr [i] = (uint8_t)(vec[i]*255);
        }
        return uint32;
    }

    int32_t vec4ToUint32(glm::u8vec4 vec){
        int32_t uint32;
        uint8_t * ptr = (uint8_t *)(&uint32);
        for (int i = 0; i < 4; i++) {
            ptr [i] = vec[i];
        }
        return uint32;
    }

    float clamp01(float value) {
        if (value < 0.f)
        {
            return 0.f;
        }
        if (value > 1.f)
        {
            return 1.f;
        }
        return value;
    }

    float repeat(float t, float length) {
        return t - floor (t / length) * length;
    }

    float lerpAngle(float a, float b, float t) {
        float num = repeat (b - a, glm::pi<float>()*2);
		if (num > glm::pi<float>())
        {
			num -= glm::pi<float>() * 2;
        }
        return a + num * clamp01 (t);
    }

    glm::vec3 faceNormal(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
        return glm::normalize(glm::cross(c - a, b - a));
    }
}