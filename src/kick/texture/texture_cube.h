//
// Created by morten on 31/05/14.
//


#pragma once

#include "kick/core/project_asset.h"
#include "kick/core/kickgl.h"
#include "kick/texture/texture_sampler.h"
#include "kick/texture/image_format.h"
#include <bitset>

namespace kick {
    class Project;

    class TextureCube : public ProjectAsset {
    public:
        TextureCube(TextureSampler textureSampler = {});
        TextureCube(const TextureCube&) = delete;
        TextureCube(TextureCube&& m);
        ~TextureCube();

        void setData(int width, int height, char* data, int cubemapIndex, const ImageFormat& imageFormat = {});

        void bind(int textureSlot);
        int getWidth(){ return width; }
        int getHeight(){ return height; }
        void setTextureSampler(const TextureSampler & textureSampler);
        TextureSampler getTextureSampler() const;
        ImageFormat getImageFormat() const;
    private:
        GLuint textureid;
        int width = -1;
        int height = -1;
        std::bitset<6> facesSet;
        TextureSampler textureSampler;
        ImageFormat imageFormat;
    };
};

