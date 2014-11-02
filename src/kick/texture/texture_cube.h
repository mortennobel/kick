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
        int width(){ return mWidth; }
        int height(){ return mHeight; }
        void setTextureSampler(const TextureSampler & textureSampler);
        TextureSampler textureSampler() const;
        ImageFormat imageFormat() const;
    private:
        GLuint mTextureid;
        int mWidth = -1;
        int mHeight = -1;
        std::bitset<6> mFacesSet;
        TextureSampler mTextureSampler;
        ImageFormat mImageFormat;
    };
};

