//
// Created by morten on 31/05/14.
//

#include "texture_cube.h"


namespace kick {
    TextureCube::TextureCube(TextureSampler textureSampler)
    {
        glGenTextures(1, &mTextureid);
        setTextureSampler(textureSampler);
    }

    TextureCube::TextureCube(TextureCube&& m)
    : mTextureid(m.mTextureid)
    {
        m.mTextureid = 0;
    }

    TextureCube::~TextureCube(){
        glDeleteTextures(1, &mTextureid);
    }

    void TextureCube::bind(int textureSlot){
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureid);
    }

    void TextureCube::setData(int width, int height, char* data, int cubemapIndex, const ImageFormat& imageFormat){
        this->mWidth = width;
        this->mHeight = height;
        this->mImageFormat = imageFormat;
        GLint border = 0; // must be 0

        glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureid);

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+cubemapIndex, imageFormat.mipmapLevel, imageFormat.internalFormat, width, height/6, border, imageFormat.format, imageFormat.type, data);
        mFacesSet[cubemapIndex] = 1;
        if (mFacesSet.all() && imageFormat.mipmap != Mipmap::None){
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        }
    }

    void TextureCube::setTextureSampler(const TextureSampler & textureSampler){
        this->mTextureSampler = textureSampler;
        glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureid);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, static_cast<GLint>(textureSampler.wrapS));
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, static_cast<GLint>(textureSampler.wrapT));
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(textureSampler.filterMag));
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(textureSampler.filterMin));

    }

    TextureSampler TextureCube::textureSampler() const{
        return mTextureSampler;
    }

    ImageFormat TextureCube::imageFormat() const {
        return mImageFormat;
    }
}