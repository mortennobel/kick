//
//  texture.cpp
//  KickCPP
//
//  Created by morten on 16/03/14.
//  Copyright (c) 2014 Morten Nobel-Joergensen. All rights reserved.
//

#include "texture2d.h"
#include "kick/core/debug.h"
#ifndef EMSCRIPTEN
#ifdef _WIN32
#include <SDL_surface.h>
#else
#include <SDL2/SDL_surface.h>
#endif
#endif
using namespace std;

namespace kick {
    Texture2D::Texture2D(TextureSampler textureSampler)
    {
        glGenTextures(1, &mTextureid);
        setTextureSampler(textureSampler);
    }
    
    Texture2D::Texture2D(Texture2D&& m)
    : mTextureid(m.mTextureid)
    {
        m.mTextureid = 0;
    }
    
    Texture2D::~Texture2D(){
        glDeleteTextures(1, &mTextureid);
    }
    
    void Texture2D::bind(int textureSlot){
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        glBindTexture(GL_TEXTURE_2D, mTextureid);
    }
    
    void Texture2D::setData(int width, int height, char* data, const ImageFormat& imageFormat){
        this->mWidth = width;
        this->mHeight = height;
        this->mImageFormat = imageFormat;
        GLenum target = GL_TEXTURE_2D;
        GLint border = 0; // must be 0

        glBindTexture(GL_TEXTURE_2D, mTextureid);

        glTexImage2D(target, imageFormat.mipmapLevel, imageFormat.internalFormat, width, height, border, imageFormat.format, imageFormat.type, data);
        if (imageFormat.mipmap != Mipmap::None){
            glGenerateMipmap(GL_TEXTURE_2D);
        }
    }

    void Texture2D::setTextureSampler(const TextureSampler & textureSampler){
        this->mTextureSampler = textureSampler;
        glBindTexture(GL_TEXTURE_2D, mTextureid);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(textureSampler.wrapS));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(textureSampler.wrapT));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(textureSampler.filterMag));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(textureSampler.filterMin));
    }

    TextureSampler Texture2D::textureSampler() const{
        return mTextureSampler;
    }
    
    ImageFormat Texture2D::imageFormat() const {
        return mImageFormat;
    }

    void Texture2D::saveBMPImage(string filename) {
        vector<char> res(mWidth * mHeight *4);
#if defined(KICK_CONTEXT_ES2)
    logWarning("Texture2D::getPngImage() is unsupported on ES2");
#elif defined(EMSCRIPTEN)
        logWarning("Texture2D::getPngImage() is unsupported on EMSCRIPTEN");
#else
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, res.data());
#endif

#ifndef EMSCRIPTEN
        SDL_Surface* sdlSurface = SDL_CreateRGBSurfaceFrom(res.data(), mWidth, mHeight,32, mWidth *4, 0xff,0xff<<8,0xff<<16,0xff<<24);

        SDL_SaveBMP(sdlSurface, filename.c_str());
        sdlSurface->pixels = nullptr;
        SDL_FreeSurface(sdlSurface);
#endif
    }


    void Texture2D::setEmptyColorTexture(int width, int height, int colorChannels, bool fpTexture) {
        ImageFormat imageFormat;
        if (fpTexture){
            if (colorChannels == 4){
                imageFormat.internalFormat = GL_RGBA32F;
            } else if (colorChannels == 3){
                imageFormat.internalFormat = GL_RGB32F;
            } else if (colorChannels == 2){
                imageFormat.internalFormat = GL_RG32F;
            } else if (colorChannels == 1){
                imageFormat.internalFormat = GL_R32F;
            }
        } else {
            if (colorChannels == 4){
                imageFormat.internalFormat = GL_RGBA8;
            } else if (colorChannels == 3){
                imageFormat.internalFormat = GL_RGB8;
            } else if (colorChannels == 2){
                imageFormat.internalFormat = GL_RG8;
            } else if (colorChannels == 1){
                imageFormat.internalFormat = GL_R8;
            }
        }
        if (colorChannels == 4){
            imageFormat.format = GL_RGBA;
        } else if (colorChannels == 3){
            imageFormat.format = GL_RGB;
        } else if (colorChannels == 2){
            imageFormat.format = GL_RG;
        } else if (colorChannels == 1){
            imageFormat.format = GL_RED;
        }
        setData(width, height, nullptr, imageFormat);
    }

    void Texture2D::setEmptyDepthTexture(int width, int height, int bits, bool fpTexture) {
        ImageFormat imageFormat;
        if (bits == 16){
            if (fpTexture){
                logWarning("Unsupported fpTexture for 16 bit depth texture");
            }
            imageFormat.internalFormat = GL_DEPTH_COMPONENT16;
        } else if (bits == 24){
            if (fpTexture){
                logWarning("Unsupported fpTexture for 24 bit depth texture");
            }
            imageFormat.internalFormat = GL_DEPTH_COMPONENT24;
        } else if (bits == 32 && fpTexture) {
            imageFormat.internalFormat = GL_DEPTH_COMPONENT32F;
        } else {
            if (bits != 32){
                logWarning("Unsupported bit depth for depth texture");
            }
            imageFormat.internalFormat = GL_DEPTH_COMPONENT32;
        }

        imageFormat.format = GL_DEPTH_COMPONENT;

        setData(width, height, nullptr, imageFormat);
    }
}