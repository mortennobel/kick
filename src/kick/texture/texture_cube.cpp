//
// Created by morten on 31/05/14.
//

#include "texture_cube.h"


namespace kick {
    TextureCube::TextureCube(Project* p, TextureSampler textureSampler)
            :ProjectAsset{p}{
        glGenTextures(1, &textureid);
        setTextureSampler(textureSampler);
    }

    TextureCube::TextureCube(TextureCube&& m)
            :ProjectAsset{m.project},textureid(m.textureid)
    {
        m.textureid = 0;
    }

    TextureCube::~TextureCube(){
        glDeleteTextures(1, &textureid);
    }

    void TextureCube::bind(int textureSlot){
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureid);
    }

    void TextureCube::setData(int width, int height, char* data, int cubemapIndex, const ImageFormat& imageFormat){
        this->width = width;
        this->height = height;
        this->imageFormat = imageFormat;
        GLenum target = GL_TEXTURE_2D;
        GLint border = 0; // must be 0

        glBindTexture(GL_TEXTURE_CUBE_MAP, textureid);

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+cubemapIndex, imageFormat.mipmapLevel, imageFormat.internalFormat, width, height/6, border, imageFormat.format, imageFormat.type, data);
        facesSet[cubemapIndex] = 1;
        if (facesSet.all() && imageFormat.mipmap != Mipmap::None){
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        }
    }

    void TextureCube::setTextureSampler(const TextureSampler & textureSampler){
        this->textureSampler = textureSampler;
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureid);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, static_cast<GLint>(textureSampler.wrapS));
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, static_cast<GLint>(textureSampler.wrapT));
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(textureSampler.filterMag));
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(textureSampler.filterMin));

    }

    TextureSampler TextureCube::getTextureSampler() const{
        return textureSampler;
    }

    ImageFormat TextureCube::getImageFormat() const {
        return imageFormat;
    }
}