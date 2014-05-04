//
//  image_format.cpp
//  UnitTest_KickCPP
//
//  Created by morten on 07/04/14.
//
//

#include "image_format.h"

using namespace std;

namespace kick {
    bool ImageFormat::isValid(std::function<void (std::string)> onError){
        bool valid = true;
        // OpenGL 4 glTexImage2D table 1
        if (internalFormat != GL_DEPTH_COMPONENT &&
            internalFormat != GL_DEPTH_STENCIL &&
            internalFormat != GL_RED &&
            internalFormat != GL_RG &&
            internalFormat != GL_RGB &&
            internalFormat != GL_RGBA){
            // OpenGL 4 glTexImage2D table 2
            if (internalFormat != GL_R8 &&
                internalFormat != GL_R8_SNORM &&
                internalFormat != GL_R16 &&
                internalFormat != GL_R16_SNORM &&
                internalFormat != GL_RG8 &&
                internalFormat != GL_RG8_SNORM &&
                internalFormat != GL_RG16 &&
                internalFormat != GL_RG16_SNORM &&
                internalFormat != GL_R3_G3_B2 &&
                internalFormat != GL_RGB4 &&
                internalFormat != GL_RGB5 &&
                internalFormat != GL_RGB8 &&
                internalFormat != GL_RGB8_SNORM &&
                internalFormat != GL_RGB10 &&
                internalFormat != GL_RGB12 &&
                internalFormat != GL_RGB16_SNORM &&
                internalFormat != GL_RGBA2 &&
                internalFormat != GL_RGBA4 &&
                internalFormat != GL_RGB5_A1 &&
                internalFormat != GL_RGBA8 &&
                internalFormat != GL_RGBA8_SNORM &&
                internalFormat != GL_RGB10_A2 &&
                internalFormat != GL_RGB10_A2UI &&
                internalFormat != GL_RGBA12 &&
                internalFormat != GL_RGBA16 &&
                internalFormat != GL_SRGB8 &&
                internalFormat != GL_SRGB8_ALPHA8 &&
                internalFormat != GL_R16F &&
                internalFormat != GL_RG16F &&
                internalFormat != GL_RGB16F &&
                internalFormat != GL_RGBA16F &&
                internalFormat != GL_R32F &&
                internalFormat != GL_RG32F &&
                internalFormat != GL_RGB32F &&
                internalFormat != GL_RGBA32F &&
                internalFormat != GL_R11F_G11F_B10F &&
                internalFormat != GL_RGB9_E5 &&
                internalFormat != GL_R8I && 
                internalFormat != GL_R8UI && 
                internalFormat != GL_R16I && 
                internalFormat != GL_R16UI && 
                internalFormat != GL_R32I && 
                internalFormat != GL_R32UI && 
                internalFormat != GL_RG8I && 
                internalFormat != GL_RG8UI && 
                internalFormat != GL_RG16I && 
                internalFormat != GL_RG16UI && 
                internalFormat != GL_RG32I && 
                internalFormat != GL_RG32UI && 
                internalFormat != GL_RGB8I && 
                internalFormat != GL_RGB8UI && 
                internalFormat != GL_RGB16I && 
                internalFormat != GL_RGB16UI && 
                internalFormat != GL_RGB32I && 
                internalFormat != GL_RGB32UI && 
                internalFormat != GL_RGBA8I && 
                internalFormat != GL_RGBA8UI && 
                internalFormat != GL_RGBA16I && 
                internalFormat != GL_RGBA16UI && 
                internalFormat != GL_RGBA32I && 
                internalFormat != GL_RGBA32UI){
                
                // OpenGL 4 glTexImage2D table 3
                if (internalFormat != GL_COMPRESSED_RED &&
                    internalFormat != GL_COMPRESSED_RG &&
                    internalFormat != GL_COMPRESSED_RGB &&
                    internalFormat != GL_COMPRESSED_RGBA &&
                    internalFormat != GL_COMPRESSED_SRGB &&
                    internalFormat != GL_COMPRESSED_SRGB_ALPHA &&
                    internalFormat != GL_COMPRESSED_RED_RGTC1 &&
                    internalFormat != GL_COMPRESSED_SIGNED_RED_RGTC1 &&
                    internalFormat != GL_COMPRESSED_RG_RGTC2 &&
                    internalFormat != GL_COMPRESSED_SIGNED_RG_RGTC2
                    /*
                    &&
                    internalFormat != GL_COMPRESSED_RGBA_BPTC_UNORM &&
                    internalFormat != GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM && 
                    internalFormat != GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT && 
                    internalFormat != GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT*/
                    ){
                    valid = false;
                    onError(string{"internalFormat has an invalid value: "}+to_string(internalFormat));
                }
                
            }
        }
        if (format != GL_RED &&
            format != GL_RG &&
            format != GL_RGB &&
            format != GL_BGR &&
            format != GL_RGBA &&
            format != GL_BGRA &&
            format != GL_RED_INTEGER &&
            format != GL_RG_INTEGER &&
            format != GL_RGB_INTEGER &&
            format != GL_BGR_INTEGER &&
            format != GL_RGBA_INTEGER &&
            format != GL_BGRA_INTEGER &&
            format != GL_STENCIL_INDEX && 
            format != GL_DEPTH_COMPONENT && 
            format != GL_DEPTH_STENCIL){
            valid = false;
            onError(string{"format has an invalid value: "}+to_string(format));
        }
        
        if (type != GL_UNSIGNED_BYTE &&
            type != GL_BYTE &&
            type != GL_UNSIGNED_SHORT &&
            type != GL_SHORT &&
            type != GL_UNSIGNED_INT &&
            type != GL_INT &&
            type != GL_FLOAT &&
            type != GL_UNSIGNED_BYTE_3_3_2 &&
            type != GL_UNSIGNED_BYTE_2_3_3_REV &&
            type != GL_UNSIGNED_SHORT_5_6_5 &&
            type != GL_UNSIGNED_SHORT_5_6_5_REV &&
            type != GL_UNSIGNED_SHORT_4_4_4_4 &&
            type != GL_UNSIGNED_SHORT_4_4_4_4_REV &&
            type != GL_UNSIGNED_SHORT_5_5_5_1 &&
            type != GL_UNSIGNED_SHORT_1_5_5_5_REV &&
            type != GL_UNSIGNED_INT_8_8_8_8 && 
            type != GL_UNSIGNED_INT_8_8_8_8_REV && 
            type != GL_UNSIGNED_INT_10_10_10_2 && 
            type != GL_UNSIGNED_INT_2_10_10_10_REV){
            valid = false;
            onError(string{"type has an invalid value: "}+to_string(type));
        }
        
        return valid;
    }
};