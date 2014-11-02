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
        glGenFramebuffers(1, &mFramebuffer);
    }

    TextureRenderTarget::~TextureRenderTarget() {
        glDeleteFramebuffers(1, &mFramebuffer);
        mFramebuffer = 0;
    }

    void TextureRenderTarget::bind() {
        assert(mRenderBuffers.size() > 0); // probably missing TextureRenderTarget.apply
        glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
    }

    void TextureRenderTarget::unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void TextureRenderTarget::apply() {
        if (mRenderBuffers.size()) {
            glDeleteRenderbuffers(mRenderBuffers.size(), mRenderBuffers.data());
            mRenderBuffers.clear();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);

        // bind color attachment
        if (mColorTextures.size() > 0){
            for (int i=0;i< mColorTextures.size();i++) {
                Texture2D* colorTexture = mColorTextures[i].get();
                glFramebufferTexture2D(GL_FRAMEBUFFER, (GLenum) (GL_COLOR_ATTACHMENT0+i), GL_TEXTURE_2D, colorTexture->mTextureid, 0);
            }
        } else {
            GLuint renderBuffer;
            glGenRenderbuffers(1, &renderBuffer);
            mRenderBuffers.push_back(renderBuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, mSize.x, mSize.y);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderBuffer);
        }

        // bind depth attachments
        GLuint renderBuffer;
        glGenRenderbuffers(1, &renderBuffer);
        mRenderBuffers.push_back(renderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER,
#ifdef EMSCRIPTEN
                GL_DEPTH_COMPONENT16,
#else
                GL_DEPTH_COMPONENT32,
#endif

                mSize.x, mSize.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

        checkStatus();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
        TextureRenderTarget::mSize = size;
    }

    glm::ivec2 TextureRenderTarget::size() const {
        return mSize;
    }

    void TextureRenderTarget::setColorTexture(size_t channel, std::shared_ptr<Texture2D> texture) {
        if (mColorTextures.size() <= channel){
            mColorTextures.resize(channel+1, nullptr);
        }
        mColorTextures[channel] = texture;
    }

    std::shared_ptr<Texture2D> TextureRenderTarget::colorTexture(size_t channel) {
        if (mColorTextures.size() <= channel){
            return nullptr;
        }
        return mColorTextures[channel];
    }

    bool TextureRenderTarget::deleteColorTexture(size_t channel) {
        if (mColorTextures.size() <= channel){
            return false;
        }
        mColorTextures[channel].reset();
        mColorTextures.erase(mColorTextures.begin() + channel);
        return true;
    }
}