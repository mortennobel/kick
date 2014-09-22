//
// Created by morten on 19/09/14.
//

#include "misc.h"

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
}