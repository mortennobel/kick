//
// Created by morten on 19/07/14.
//


#pragma once
#include "kick/scene/updatable.h"
#include "kick/scene/component.h"
#include "kick/2d/sprite.h"
#include "kick/2d/text.h"
#include <string>
#include <functional>

namespace kick {
    class TextureAtlas;

    enum class ButtonState {
        normal, hover, pressed
    };

    class Button : public Component, public Updateable {
    public:
        Button(GameObject *gameObject) : Component(gameObject) {
        }

        std::string getNormal() const;
        void setNormal(std::string const &normal);
        std::string getHover() const;
        void setHover(std::string const &hover);
        std::string getPressed() const;
        void setPressed(std::string const &pressed);
        std::string getText() const;
        void setText(std::string const &text);
        std::function<void(Button*)> const & getOnClick() const;
        void setOnClick(std::function<void(Button*)> const &onClick);

        virtual void activated() override;

        virtual void deactivated() override;

        virtual void update();

        std::shared_ptr<TextureAtlas> getTextureAtlas() const;
        void setTextureAtlas(std::shared_ptr<TextureAtlas> textureAtlas);

    private:
        std::shared_ptr<TextureAtlas> textureAtlas;
        void updateTexture();
        ButtonState state = ButtonState::normal;
        std::string normal;
        std::string hover;
        std::string pressed;
        std::string text;
        std::function<void(Button*)> onClick;
        Sprite *sprite = nullptr;
        Text *textComponent = nullptr;
    };
}


