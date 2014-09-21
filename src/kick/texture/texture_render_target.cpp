//
// Created by morten on 05/06/14.
//

#include "texture_render_target.h"

#include <iostream>
#include <string>

using namespace std;

namespace kick {
    TextureRenderTarget::TextureRenderTarget()
    {
        glGenFramebuffers(1, &framebuffer);
    }

    TextureRenderTarget::~TextureRenderTarget() {
        glDeleteFramebuffers(1, &framebuffer);
        framebuffer = 0;
    }

    void TextureRenderTarget::bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    }

    void TextureRenderTarget::unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void TextureRenderTarget::apply() {
        if (renderBuffers.size()) {
            glDeleteRenderbuffers(renderBuffers.size(), renderBuffers.data());
            renderBuffers.clear();
        }
        bind();

        // bind color attachment
        if (colorTextures.size() > 0){
            for (int i=0;i<colorTextures.size();i++) {
                Texture2D* colorTexture = colorTextures[i].get();
                glFramebufferTexture2D(GL_FRAMEBUFFER, (GLenum) (GL_COLOR_ATTACHMENT0+i), GL_TEXTURE_2D, colorTexture->textureid, 0);
            }
        } else {
            GLuint renderBuffer;
            glGenRenderbuffers(1, &renderBuffer);
            renderBuffers.push_back(renderBuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, size.x, size.y);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderBuffer);
        }

        // bind depth attachments
        GLuint renderBuffer;
        glGenRenderbuffers(1, &renderBuffer);
        renderBuffers.push_back(renderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, size.x, size.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

        checkStatus();

        unbind();
    }

    void TextureRenderTarget::checkStatus() {
        GLenum frameBufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (frameBufferStatus != GL_FRAMEBUFFER_COMPLETE) {
            switch (frameBufferStatus) {
                case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                    cerr << ("GL_FRAMEBUFFER_UNDEFINED");
                    break;
#ifdef GL_ES_VERSION_2_0
                case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
                    cerr << ("GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS");
                    break;
#endif
                case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                    cerr << ("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
                    break;
                case GL_FRAMEBUFFER_UNSUPPORTED:
                    cerr << ("FRAMEBUFFER_UNSUPPORTED");
                    break;
#ifndef GL_ES_VERSION_2_0
                case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                    cerr << ("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
                    break;
                case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                    cerr << ("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER");
                    break;
                case GL_FRAMEBUFFER_UNDEFINED:
                    cerr << ("FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
                    break;
                case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                    cerr << ("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
                    break;
                case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
                    cerr << ("GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS");
                    break;
#endif
                default:
                    cerr << string("Invalid framebuffer ")+ __1::to_string(frameBufferStatus);
                    break;
            }
        }
    }

    void TextureRenderTarget::setSize(glm::ivec2 size) {
        TextureRenderTarget::size = size;
    }

    glm::ivec2 TextureRenderTarget::getSize() const {
        return size;
    }

    void TextureRenderTarget::setColorTexture(size_t channel, std::shared_ptr<Texture2D> texture) {
        if (colorTextures.size() <= channel){
            colorTextures.resize(channel+1, nullptr);
        }
        colorTextures[channel] = texture;
    }

    std::shared_ptr<Texture2D> TextureRenderTarget::getColorTexture(size_t channel) {
        if (colorTextures.size() <= channel){
            return nullptr;
        }
        return colorTextures[channel];
    }

    bool TextureRenderTarget::deleteColorTexture(size_t channel) {
        if (colorTextures.size() <= channel){
            return false;
        }
        colorTextures[channel].reset();
        colorTextures.erase(colorTextures.begin() + channel);
        return true;
    }
}