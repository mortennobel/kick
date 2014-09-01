//
//  math.cpp
//  KickCPP
//
//  Created by morten on 8/13/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/math/glm_ext.h"



using namespace glm;

namespace kick {
    glm::mat4 TRS(glm::vec3 translate, glm::quat rotateQuat, glm::vec3 scale){
        return glm::translate(translate) * mat4_cast(rotateQuat) * glm::scale(scale);
        /*glm::mat4 res;
        float *out = &(res[0][0]);
        // Quaternion math
        float scaleX = scale[0], scaleY = scale[1], scaleZ = scale[2],
        x = rotateQuat[1], y = rotateQuat[2], z = rotateQuat[3], w = rotateQuat[0],
        x2 = x + x,
        y2 = y + y,
        z2 = z + z,
        
        xx = x * x2,
        xy = x * y2,
        xz = x * z2,
        yy = y * y2,
        yz = y * z2,
        zz = z * z2,
        wx = w * x2,
        wy = w * y2,
        wz = w * z2;
        
        out[0] = (1 - (yy + zz)) * scaleX;
        out[1] = (xy + wz) * scaleX;
        out[2] = (xz - wy) * scaleX;
        out[3] = 0;
        out[4] = (xy - wz) * scaleY;
        out[5] = (1 - (xx + zz)) * scaleY;
        out[6] = (yz + wx) * scaleY;
        out[7] = 0;
        out[8] = (xz + wy) * scaleZ;
        out[9] = (yz - wx) * scaleZ;
        out[10] = (1 - (xx + yy)) * scaleZ;
        out[11] = 0;
        out[12] = translate[0];
        out[13] = translate[1];
        out[14] = translate[2];
        out[15] = 1;
        
        return res;       */
    }
    
    glm::mat4 TRSInverse(glm::vec3 translate, glm::quat rotateQuat, glm::vec3 scale){
        return inverse(TRS(translate, rotateQuat, scale));
        /*
        // Quaternion math
        float scaleX = scale.x, scaleY = scale.y, scaleZ = scale.z,
        x = rotateQuat.x, y = rotateQuat.y, z = rotateQuat.z, w = rotateQuat.w,
        x2 = x + x,
        y2 = y + y,
        z2 = z + z,
        
        xx = x * x2,
        xy = x * y2,
        xz = x * z2,
        yy = y * y2,
        yz = y * z2,
        zz = z * z2,
        wx = w * x2,
        wy = w * y2,
        wz = w * z2,
        
        // compute trs
        a00 = (1 - (yy + zz)) * scaleX,
        a01 = (xy + wz) * scaleX,
        a02 = (xz - wy) * scaleX,
        a10 = (xy - wz) * scaleY,
        a11 = (1 - (xx + zz)) * scaleY,
        a12 = (yz + wx) * scaleY,
        a20 = (xz + wy) * scaleZ,
        a21 = (yz - wx) * scaleZ,
        a22 = (1 - (xx + yy)) * scaleZ,
        a30 = translate[0],
        a31 = translate[1],
        a32 = translate[2],
        a33 = 1,
        // compute inverse
        b00 = a00 * a11 - a01 * a10,
        b01 = a00 * a12 - a02 * a10,
        b03 = a01 * a12 - a02 * a11,
        b06 = a20 * a31 - a21 * a30,
        b07 = a20 * a32 - a22 * a30,
        b08 = a20 * a33,
        b09 = a21 * a32 - a22 * a31,
        b10 = a21 * a33,
        b11 = a22 * a33,
        
        d = (b00 * b11 - b01 * b10 + b03 * b08),
        invDet;
        
        mat4 res;
        float *out = &(res[0][0]);
        // Calculate the determinant
        if (!d) { return res; }
        invDet = 1 / d;
        
        out[0] = (a11 * b11 - a12 * b10) * invDet;
        out[1] = (-a01 * b11 + a02 * b10) * invDet;
        out[2] = (a33 * b03) * invDet;
        out[3] = 0;
        out[4] = (-a10 * b11 + a12 * b08) * invDet;
        out[5] = (a00 * b11 - a02 * b08) * invDet;
        out[6] = (-a33 * b01) * invDet;
        out[7] = 0;
        out[8] = (a10 * b10 - a11 * b08) * invDet;
        out[9] = (-a00 * b10 + a01 * b08) * invDet;
        out[10] = (a33 * b00) * invDet;
        out[11] = 0;
        out[12] = (-a10 * b09 + a11 * b07 - a12 * b06) * invDet;
        out[13] = (a00 * b09 - a01 * b07 + a02 * b06) * invDet;
        out[14] = (-a30 * b03 + a31 * b01 - a32 * b00) * invDet;
        out[15] = (a20 * b03 - a21 * b01 + a22 * b00) * invDet;
        
        return res;   */
    }

    glm::quat lookAt(glm::vec3 eyePosition, glm::vec3 lookAtPos, glm::vec3 up){
        return quat_cast(glm::lookAt(eyePosition, lookAtPos, up));
    }
}