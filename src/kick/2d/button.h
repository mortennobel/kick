//
// Created by morten on 19/07/14.
//


#pragma once
#include "kick/scene/updatable.h"
#include "kick/scene/component.h"
#include "kick/2d/sprite_mouse_listener.h"
#include "kick/2d/sprite.h"
#include "label.h"
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

        std::string normalSprite() const;
        void setNormalSprite(std::string const &normal);
        std::string hoverSprite() const;
        void setHoverSprite(std::string const &hover);
        std::string pressedSprite() const;
        void setPressedSprite(std::string const &pressed);
        std::string text() const;
        void setText(std::string const &text);
        std::function<void(Button*)> const & getOnClick() const;
        void setOnClick(std::function<void(Button*)> const &onClick);

        virtual void deactivated() override;
        virtual void setOrder(int order) override;

        glm::vec4 const &normalColor() const;
        void setNormalColor(glm::vec4 const &normalColor);
        glm::vec4 const &hoverColor() const;
        void setHoverColor(glm::vec4 const &hoverColor);
        glm::vec4 const &pressedColor() const;
        void setPressedColor(glm::vec4 const &pressedColor);
    protected:
        virtual std::string currentSpriteName();
        virtual void updateTextureAndTxtColor();
        virtual void down(int button) override;
        virtual void pressed(int button) override;
        virtual void up(int button) override;
        virtual void over() override;
        virtual void out() override;

        virtual void invokeClick();

        std::string mNormalName;
        std::string mHoverName;
        std::string mPressedName;

        ButtonState mState = ButtonState::normal;

        glm::vec4 mNormalColor = glm::vec4{1,1,1,1};
        glm::vec4 mHoverColor = glm::vec4{1,1,1,1};
        glm::vec4 mPressedColor = glm::vec4{0,0,0,1};
        std::string mText;
        std::function<void(Button*)> onClick;
        std::shared_ptr<Label> mTextComponent;
        std::set<int> mPressedButtons;
    };
}


