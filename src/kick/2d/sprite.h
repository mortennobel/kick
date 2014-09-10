//
// Created by morten on 30/06/14.
//

#include "glm/glm.hpp"
#include "kick/2d/component2d.h"
#include "kick/scene/game_object.h"
#include "kick/texture/texture_atlas.h"
#include <string>
#include "kick/core/event.h"
#include "bounds2.h"

#pragma once

namespace kick {
    class SpriteManager;
    class TextureAtlas;

    enum class SpriteType {
        Simple,
        // sliced in 9-patch
        Sliced
    };

    class Sprite : public Component2D {
    public:
        Sprite(GameObject *gameObject);

        std::shared_ptr<TextureAtlas> getTextureAtlas() const;

        void setTextureAtlas(std::shared_ptr<TextureAtlas> textureAtlas);

        std::string const & getSpriteName() const;

        void setSpriteName(std::string const &spriteName);

        Event<Sprite*> spriteUpdated;

        TextureAtlasEntry getEntry() const;

        Bounds2 getTrimmedBounds() const;
        Bounds2 getBounds() const;

        virtual Shader *getShader() const;

// when sprite is sliced slice x is two relative slice positions (between 0.0 and 1.0)
        glm::vec2 getSliceX() const;
        // when sprite is sliced slice x is two relative slice positions (between 0.0 and 1.0)
        void setSliceX(glm::vec2 sliceX);
        // when sprite is sliced slice y is two relative slice positions (between 0.0 and 1.0)
        glm::vec2 getSliceY() const;
        // when sprite is sliced slice y is two relative slice positions (between 0.0 and 1.0)
        void setSliceY(glm::vec2 sliceY);
        // anchor (or pivot point). Default is (0.0) which is the lower left corner.
        glm::vec2 getAnchor() const;
        void setAnchor(glm::vec2 anchor);
        glm::vec2 getScale() const;
        // scale is useful when type is sliced
        void setScale(glm::vec2 scale);
        SpriteType getType() const;
        void setType(SpriteType type);

        glm::vec4 getColor() const;
        void setColor(glm::vec4 color);
    private:
        std::shared_ptr<TextureAtlas> textureAtlas = nullptr;
        std::string spriteName;
        TextureAtlasEntry entry;
        glm::vec2 sliceX{0.333f, 0.667f};
        glm::vec2 sliceY{0.333f, 0.667f};
        glm::vec2 anchor{0,0};
        glm::vec2 scale{1,1};
        glm::vec4 color{1,1,1,1};
        SpriteType type = SpriteType::Simple;
    };
}



