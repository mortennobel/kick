//
// Created by morten on 19/07/14.
//


#pragma once
#include "kick/scene/updatable.h"
#include "kick/scene/component.h"
#include "kick/2d/sprite_mouse_listener.h"
#include "kick/2d/sprite.h"
#include "kick/2d/text.h"
#include <string>
#include <functional>
#include <set>

namespace kick {
    class TextureAtlas;

    enum class ButtonState {
        normal, hover
    };

    class Button : public Sprite, public SpriteMouseListener {
    public:
        Button(GameObject *gameObject);

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
        virtual void setOrder(int order) override;
    private:
        std::string currentSpriteName();
        virtual void down(int button) override;
        virtual void pressed(int button) override;
        virtual void up(int button) override;
        virtual void over() override;
        virtual void out() override;

        std::shared_ptr<TextureAtlas> textureAtlas;
        void updateTexture();
        ButtonState state = ButtonState::normal;
        std::string normalName;
        std::string hoverName;
        std::string pressedName;
        std::string text;
        std::function<void(Button*)> onClick;
        Text *textComponent = nullptr;
        std::set<int> pressedButtons;
    };
}


