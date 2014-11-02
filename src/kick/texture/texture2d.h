//
//  texture.h
//  KickCPP
//
//  Created by morten on 16/03/14.
//  Copyright (c) 2014 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once

#include <vector>

#include "kick/core/project_asset.h"
#include "kick/core/kickgl.h"
#include "kick/texture/texture_sampler.h"
#include "kick/texture/image_format.h"


namespace kick {
    class Project;
    
    class Texture2D : public ProjectAsset {
    public:
        Texture2D(TextureSampler textureSampler = {});
        Texture2D(const Texture2D&) = delete;
        Texture2D(Texture2D&& m);
        ~Texture2D();
        
        void setData(int width, int height, char* data, const ImageFormat& imageFormat = {});
        
        void bind(int textureSlot);
        int width(){ return mWidth; }
        int height(){ return mHeight; }
        void setTextureSampler(const TextureSampler & textureSampler);
        TextureSampler textureSampler() const;
        ImageFormat imageFormat() const;
        friend class TextureRenderTarget;

        void saveBMPImage(std::string filename);
    private:
        GLuint mTextureid;
        int mWidth = -1;
        int mHeight = -1;
        TextureSampler mTextureSampler;
        ImageFormat mImageFormat;
    };
}