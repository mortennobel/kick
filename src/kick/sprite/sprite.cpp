//
// Created by morten on 30/06/14.
//

#include "sprite.h"
#include "spritemanager.h"

#include <iostream>
#include <glm/gtx/string_cast.hpp>

using namespace std;
using namespace glm;

namespace kick {

    TextureAtlas *Sprite::getTextureAtlas() const {
        return textureAtlas;
    }

    void Sprite::setTextureAtlas(TextureAtlas *textureAtlas) {
        Sprite::textureAtlas = textureAtlas;
        if (textureAtlas && spriteName.length()>0){
            entry = textureAtlas->get(spriteName);
            spriteUpdated.notifyListeners(this);
        }
    }


    std::string const &Sprite::getSpriteName() const {
        return spriteName;
    }

    void Sprite::setSpriteName(std::string const &spriteName) {
        Sprite::spriteName = spriteName;
        if (textureAtlas){
            entry = textureAtlas->get(spriteName);
            spriteUpdated.notifyListeners(this);
        }
    }

    void Sprite::activated() {
        spriteManager = SpriteManager::getInstance(gameObject->getScene());
        spriteManager->registerSprite(this);
    }

    void Sprite::deactivated() {
        spriteManager->deregisterSprite(this);
    }

    TextureAtlasEntry Sprite::getEntry() const {
        return entry;
    }

    int Sprite::getOrder() const {
        return order;
    }

    void Sprite::setOrder(int order) {
        Sprite::order = order;
    }

    glm::vec2 Sprite::getSliceX() const {
        return sliceX;
    }

    void Sprite::setSliceX(glm::vec2 sliceX) {
        assert (sliceX.x <= sliceX.y);
        assert (0 <= sliceX.x);
        assert (sliceX.y <= 1);
        Sprite::sliceX = sliceX;
    }

    glm::vec2 Sprite::getSliceY() const {
        return sliceY;
    }

    void Sprite::setSliceY(glm::vec2 sliceY) {
        assert (sliceY.x <= sliceY.y);
        assert (0 <= sliceY.x);
        assert (sliceY.y <= 1);
        Sprite::sliceY = sliceY;
    }

    glm::vec2 Sprite::getAnchor() const {
        return anchor;
    }

    void Sprite::setAnchor(glm::vec2 anchor) {
        Sprite::anchor = anchor;
    }

    glm::vec2 Sprite::getScale() const {
        return scale;
    }

    void Sprite::setScale(glm::vec2 scale) {
        Sprite::scale = scale;
    }

    void Sprite::setType(SpriteType type) {
        Sprite::type = type;
    }

    SpriteType Sprite::getType() const {
        return type;
    }

    Bounds2D Sprite::getBounds() const {
        Bounds2D bounds;

        float w = entry.spriteSourceSize.z * scale.x;
        float h = entry.spriteSourceSize.w * scale.y;
        bounds.min = -vec2{entry.spriteSourceSize.z, entry.spriteSourceSize.w} * anchor * scale;
        bounds.max = bounds.min + glm::vec2{w,h};
        return bounds;
    }

    Bounds2D Sprite::getTrimmedBounds() const {
        Bounds2D bounds;

        float w = entry.frame.z * scale.x;
        float h = entry.frame.w * scale.y;
        bounds.min = {entry.spriteSourceSize.x, entry.spriteSourceSize.w - entry.spriteSourceSize.y - entry.frame.w};
        bounds.min -= vec2{entry.spriteSourceSize.z, entry.spriteSourceSize.w} * anchor * scale;
        bounds.max = bounds.min + glm::vec2{w,h};
        return bounds;
    }

    glm::vec4 Sprite::getColor() const {
        return color;
    }

    void Sprite::setColor(glm::vec4 color) {
        Sprite::color = color;
    }
}