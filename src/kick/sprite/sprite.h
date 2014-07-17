//
// Created by morten on 30/06/14.
//

#include "glm/glm.hpp"
#include "kick/scene/component.h"
#include "kick/scene/game_object.h"
#include "kick/texture/texture_atlas.h"
#include <string>
#include "kick/core/event.h"
#pragma once

namespace kick {
    class TextureAtlas;
    class SpriteManager;

    class Sprite : public Component {
    public:
        Sprite(GameObject *gameObject) : Component(gameObject) {
        }

        virtual void activated() override;

        virtual void deactivated() override;

        TextureAtlas * getTextureAtlas() const;

        void setTextureAtlas(TextureAtlas *textureAtlas);

        std::string const & getSpriteName() const;

        void setSpriteName(std::string const &spriteName);

        Event<Sprite*> spriteUpdated;

        TextureAtlasEntry getEntry() const;

        int getOrder() const;
        void setOrder(int order);
    private:
        SpriteManager * spriteManager;
        TextureAtlas * textureAtlas = nullptr;
        std::string spriteName;
        TextureAtlasEntry entry;
        int order = 0;
    };
}



