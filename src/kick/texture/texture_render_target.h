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
        glm::ivec2 getSize() const;
        void setSize(glm::ivec2 size);
        void setColorTexture(size_t channel, std::shared_ptr<Texture2D> texture);
        std::shared_ptr<Texture2D> getColorTexture(size_t channel);
        bool deleteColorTexture(size_t channel);
        void apply();
    private:
        GLuint framebuffer = 0;
        glm::ivec2 size = glm::ivec2{512,512};
        std::vector<std::shared_ptr<Texture2D>> colorTextures;
        std::shared_ptr<Texture2D>  depthTexture;
        std::vector<GLuint> renderBuffers;
        void checkStatus();
    };

}


