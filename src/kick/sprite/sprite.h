//
// Created by morten on 30/06/14.
//

#include "glm/glm.hpp"
#include "kick/scene/component.h"
#include "kick/scene/game_object.h"
#include "kick/texture/texture_atlas.h"
#include <string>

#pragma once

namespace kick {
    class TextureAtlas;

    class Sprite : public Component{
    public:
        Sprite(GameObject *gameObject) : Component(gameObject) {
        }

        TextureAtlas * getTextureAtlas() const;

        void setTextureAtlas(TextureAtlas *textureAtlas);

        std::string const & getSpriteName() const;

        void setSpriteName(std::string const &spriteName);

    private:
        TextureAtlas * textureAtlas = nullptr;
        std::string spriteName;

    };
}



