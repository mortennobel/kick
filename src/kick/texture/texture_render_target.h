//
// Created by morten on 05/06/14.
//


#include "kick/texture/texture2d.h"
#include "kick/core/project.h"
#include "glm/glm.hpp"
#include <vector>

#pragma once

namespace kick {
    class Texture2D;

    class TextureRenderTarget : public ProjectAsset {
    public:
        TextureRenderTarget();
        ~TextureRenderTarget();
        void bind();
        void unbind();
        glm::ivec2 size() const;
        void setSize(glm::ivec2 size);
        void setColorTexture(size_t channel, std::shared_ptr<Texture2D> texture);
        void setDepthTexture(std::shared_ptr<Texture2D> texture);
        std::shared_ptr<Texture2D> colorTexture(size_t channel);
        bool deleteColorTexture(size_t channel);
        bool deleteDepthTexture();
        void apply();
    private:
        GLuint mFramebuffer = 0;
        glm::ivec2 mSize = glm::ivec2{512,512};
        std::vector<std::shared_ptr<Texture2D>> mColorTextures;
        std::shared_ptr<Texture2D> mDepthTexture;
        std::vector<GLuint> mRenderBuffers;
        void checkStatus();
    };

}


