//
// Created by morten on 30/06/14.
//

#include "sprite.h"

namespace kick {

    TextureAtlas *Sprite::getTextureAtlas() const {
        return textureAtlas;
    }

    void Sprite::setTextureAtlas(TextureAtlas *textureAtlas) {
                Sprite::textureAtlas = textureAtlas;
            }


    std::string const &Sprite::getSpriteName() const {
        return spriteName;
    }

    void Sprite::setSpriteName(std::string const &spriteName) {
        Sprite::spriteName = spriteName;
    }
}