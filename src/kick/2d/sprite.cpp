//
// Created by morten on 30/06/14.
//

#include "kick/2d/sprite.h"
#include "kick/core/debug.h"

#include <iostream>
#include <glm/gtx/string_cast.hpp>

using namespace std;
using namespace glm;

namespace kick {

    Sprite::Sprite(GameObject *gameObject, std::shared_ptr<Canvas> canvas) : Component2D(gameObject, canvas) {
    }

    std::shared_ptr<TextureAtlas> Sprite::textureAtlas() const {
        return mTextureAtlas;
    }

    void Sprite::setTextureAtlas(std::shared_ptr<TextureAtlas> textureAtlas) {
        Sprite::mTextureAtlas = textureAtlas;
        if (textureAtlas && mSpriteName.length()>0){
            mEntry = textureAtlas->get(mSpriteName);
        }
    }


    std::string const &Sprite::spriteName() const {
        return mSpriteName;
    }

    void Sprite::setSpriteName(std::string const &spriteName) {
        Sprite::mSpriteName = spriteName;
        if (mTextureAtlas && spriteName != ""){
            mEntry = mTextureAtlas->get(spriteName);
        }
    }

    TextureAtlasEntry Sprite::entry() const {
        return mEntry;
    }

    glm::vec2 Sprite::sliceX() const {
        return mSliceX;
    }

    void Sprite::setSliceX(glm::vec2 sliceX) {
        assert (sliceX.x <= sliceX.y);
        assert (0 <= sliceX.x);
        assert (sliceX.y <= 1);
        Sprite::mSliceX = sliceX;
    }

    glm::vec2 Sprite::sliceY() const {
        return mSliceY;
    }

    void Sprite::setSliceY(glm::vec2 sliceY) {
        assert (sliceY.x <= sliceY.y);
        assert (0 <= sliceY.x);
        assert (sliceY.y <= 1);
        Sprite::mSliceY = sliceY;
    }

    glm::vec2 Sprite::anchor() const {
        return mAnchor;
    }

    void Sprite::setAnchor(glm::vec2 anchor) {
        Sprite::mAnchor = anchor;
    }

    glm::vec2 Sprite::scale() const {
        return mScale;
    }

    void Sprite::setScale(glm::vec2 scale) {
        Sprite::mScale = scale;
    }

    void Sprite::setType(SpriteType type) {
        Sprite::mType = type;
    }

    SpriteType Sprite::type() const {
        return mType;
    }

    Bounds2 Sprite::bounds() const {
        Bounds2 bounds;

        float w = mEntry.spriteSourceSize.z * mScale.x;
        float h = mEntry.spriteSourceSize.w * mScale.y;
        bounds.min = -vec2{mEntry.spriteSourceSize.z, mEntry.spriteSourceSize.w} * mAnchor * mScale;
        bounds.max = bounds.min + glm::vec2{w,h};
        return bounds;
    }

    Bounds2 Sprite::trimmedBounds() const {
        Bounds2 bounds;

        float w = mEntry.frame.z * mScale.x;
        float h = mEntry.frame.w * mScale.y;
        bounds.min = {mEntry.spriteSourceSize.x, mEntry.spriteSourceSize.w - mEntry.spriteSourceSize.y - mEntry.frame.w};
        bounds.min -= vec2{mEntry.spriteSourceSize.z, mEntry.spriteSourceSize.w} * mAnchor * mScale;
        bounds.max = bounds.min + glm::vec2{w,h};
        return bounds;
    }

    glm::vec4 Sprite::color() const {
        return mColor;
    }

    void Sprite::setColor(glm::vec4 color) {
        Sprite::mColor = color;
    }

    Shader *Sprite::shader() const {
        if (mTextureAtlas){
            return mTextureAtlas->shader().get();
        }
        return nullptr;
    }

    void Sprite::setBounds(Bounds2 b) {
        cout << "Todo implement"<<endl;
    }
}