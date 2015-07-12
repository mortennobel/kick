//
// Created by morten on 30/06/14.
//

#include "glm/glm.hpp"
#include "kick/2d/component2d.h"
#include "kick/scene/game_object.h"
#include "kick/texture/texture_atlas.h"
#include <string>
#include "kick/core/event.h"
#include "kick/math/bounds2.h"

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
        Sprite(GameObject *gameObject, std::shared_ptr<Canvas> canvas);

        std::shared_ptr<TextureAtlas> textureAtlas() const;

        void setTextureAtlas(std::shared_ptr<TextureAtlas> textureAtlas);

        std::string const &spriteName() const;

        void setSpriteName(std::string const &spriteName);

        TextureAtlasEntry entry() const;

        Bounds2 trimmedBounds() const;
        virtual Bounds2 bounds() const override;
        virtual void setBounds(Bounds2 b) override;

        virtual Shader *shader() const override;

        // when sprite is sliced slice x is two relative slice positions (between 0.0 and 1.0)
        glm::vec2 sliceX() const;
        // when sprite is sliced slice x is two relative slice positions (between 0.0 and 1.0)
        void setSliceX(glm::vec2 sliceX);
        // when sprite is sliced slice y is two relative slice positions (between 0.0 and 1.0)
        glm::vec2 sliceY() const;
        // when sprite is sliced slice y is two relative slice positions (between 0.0 and 1.0)
        void setSliceY(glm::vec2 sliceY);
        // anchor (or pivot point). Default is (0.0) which is the lower left corner.
        glm::vec2 anchor() const;
        void setAnchor(glm::vec2 anchor);
        glm::vec2 scale() const;
        // scale is useful when type is sliced
        void setScale(glm::vec2 scale);
        SpriteType type() const;
        void setType(SpriteType type);

        glm::vec4 color() const;
        void setColor(glm::vec4 color);
    private:
        std::shared_ptr<TextureAtlas> mTextureAtlas = nullptr;
        std::string mSpriteName;
        TextureAtlasEntry mEntry;
        glm::vec2 mSliceX{0.333f, 0.667f};
        glm::vec2 mSliceY{0.333f, 0.667f};
        glm::vec2 mAnchor{0,0};
        glm::vec2 mScale{1,1};
        glm::vec4 mColor{1,1,1,1};
        SpriteType mType = SpriteType::Simple;
    };
}



