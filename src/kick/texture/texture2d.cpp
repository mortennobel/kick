//
//  texture.cpp
//  KickCPP
//
//  Created by morten on 16/03/14.
//  Copyright (c) 2014 Morten Nobel-Joergensen. All rights reserved.
//

#include "texture2d.h"

namespace kick {
    Texture2D::Texture2D(Project* p, TextureSampler textureSampler)
    :ProjectAsset{p}{
        glGenTextures(1, &textureid);
        setTextureSampler(textureSampler);
    }
    
    Texture2D::Texture2D(Texture2D&& m)
    :ProjectAsset{m.project},textureid(m.textureid)
    {
        m.textureid = 0;
    }
    
    Texture2D::~Texture2D(){
        glDeleteTextures(1, &textureid);
    }
    
    void Texture2D::bind(int textureSlot){
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        glBindTexture(GL_TEXTURE_2D, textureid);
    }
    
    void Texture2D::setData(int width, int height, char* data, const ImageFormat& imageFormat){
        this->width = width;
        this->height = height;
        this->imageFormat = imageFormat;
        GLenum target = GL_TEXTURE_2D;
        GLint border = 0; // must be 0

        glBindTexture(GL_TEXTURE_2D, textureid);

        glTexImage2D(target, imageFormat.mipmapLevel, imageFormat.internalFormat, width, height, border, imageFormat.format, imageFormat.type, data);
        if (imageFormat.mipmap != Mipmap::None){
            glGenerateMipmap(GL_TEXTURE_2D);
        }
    }

    void Texture2D::setTextureSampler(const TextureSampler & textureSampler){
        this->textureSampler = textureSampler;
        glBindTexture(GL_TEXTURE_2D, textureid);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(textureSampler.wrapS));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(textureSampler.wrapT));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(textureSampler.filterMag));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(textureSampler.filterMin));

    }

    TextureSampler Texture2D::getTextureSampler() const{
        return textureSampler;
    }
    
    ImageFormat Texture2D::getImageFormat() const {
        return imageFormat;
    }
}