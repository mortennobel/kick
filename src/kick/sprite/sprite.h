//
// Created by morten on 30/06/14.
//

#include "glm/glm.hpp"
#include "kick/scene/component.h"
#include "kick/scene/game_object.h"
#include "kick/texture/texture_atlas.h"
#include <string>
#include "kick/core/event.h"
#include "kick/math/bounds2d.h"

#pragma once

namespace kick {
    class SpriteManager;
    class TextureAtlas;

    enum class SpriteType {
        Simple,
        Sliced
    };

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

        Bounds2D getTrimmedBounds() const;
        Bounds2D getBounds() const;

        glm::vec2 getSliceX() const;
        void setSliceX(glm::vec2 sliceX);
        glm::vec2 getSliceY() const;
        void setSliceY(glm::vec2 sliceY);
        glm::vec2 getAnchor() const;
        void setAnchor(glm::vec2 anchor);
        glm::vec2 getScale() const;
        void setScale(glm::vec2 scale);
        SpriteType getType() const;
        void setType(SpriteType type);

        glm::vec4 getColor() const;
        void setColor(glm::vec4 color);
    private:
        SpriteManager * spriteManager;
        TextureAtlas * textureAtlas = nullptr;
        std::string spriteName;
        TextureAtlasEntry entry;
        int order = 0;
        glm::vec2 sliceX{0.333f, 0.667f};
        glm::vec2 sliceY{0.333f, 0.667f};
        glm::vec2 anchor{0,0};
        glm::vec2 scale{1,1};
        glm::vec4 color{1,1,1,1};
        SpriteType type = SpriteType::Simple;
    };
}



