//
// Created by morten on 30/06/14.
//

#include "sprite.h"
#include "spritemanager.h"

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
}