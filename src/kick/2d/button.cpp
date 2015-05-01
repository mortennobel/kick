//
// Created by morten on 19/07/14.
//

#include "button.h"
#include "kick/scene/scene.h"
#include "kick/scene/transform.h"
#include "canvas.h"
#include <iostream>

using namespace std;

namespace kick {
    Button::Button(GameObject *gameObject)
            : Sprite{gameObject},
              onClick{[](Button*){}}
    {
        setType(SpriteType::Sliced);
        setAnchor({0.5f,0.5f});
        mTextComponent = mCanvas->createLabel(mText);
        mTextComponent->transform()->setParent(transform());
        mTextComponent->setText(mText);
        mTextComponent->setAnchor({0.5f,0.5f});
        mTextComponent->setOrder(order() + 1);
        setOrder(order()); // force setting render order of text
        updateTextureAndTxtColor();
    }

    void Button::setText(std::string const &text) {
        Button::mText = text;
        if (mTextComponent){
            mTextComponent->setText(text);
        }
    }

    std::string Button::text() const {
        return mText;
    }

    void Button::setPressedSprite(std::string const &pressed) {
        Button::mPressedName = pressed;
        updateTextureAndTxtColor();
    }

    std::string Button::pressedSprite() const {
        return mPressedName;
    }

    void Button::setHoverSprite(std::string const &hover) {
        Button::mHoverName = hover;
        updateTextureAndTxtColor();
    }

    std::string Button::hoverSprite() const {
        return mHoverName;
    }

    void Button::setNormalSprite(std::string const &normal) {
        Button::mNormalName = normal;
        updateTextureAndTxtColor();
    }

    std::string Button::normalSprite() const {
        return mNormalName;
    }

    void Button::setOnClick(std::function<void(Button*)> const &onClick) {
        Button::onClick = onClick;
    }

    std::function<void(Button*)> const &Button::getOnClick() const {
        return onClick;
    }

    void Button::updateTextureAndTxtColor() {
        setSpriteName(currentSpriteName());
        if (mPressedButtons.size()>0){
            mTextComponent->material()->setUniform("mainColor", mPressedColor);
        } else {
            if (mState == ButtonState::normal){
                mTextComponent->material()->setUniform("mainColor", mNormalColor);
            } else {
                mTextComponent->material()->setUniform("mainColor", mHoverColor);
            }
        }
    }

    std::string Button::currentSpriteName() {
        if (mPressedButtons.size()>0){
            return mPressedName;
        }
        switch (mState){
            case ButtonState::normal:
                return mNormalName;
            case ButtonState::hover:
                return mHoverName;
            default: ////case ButtonState::pressed:
                return mPressedName;
        }
    }



    void Button::deactivated() {
        Sprite::deactivated();
        gameObject()->scene()->destroyGameObject(mTextComponent->gameObject());
    }

    void Button::down(int button) {
        mPressedButtons.insert(button);
        updateTextureAndTxtColor();
        invokeClick();
    }

    void Button::pressed(int button) {

    }

    void Button::up(int button) {
        mPressedButtons.erase(button);
        updateTextureAndTxtColor();
    }

    void Button::over() {
        mState = ButtonState::hover;
        updateTextureAndTxtColor();
    }

    void Button::out() {
        mState = ButtonState::normal;
        updateTextureAndTxtColor();
    }

    void Button::setOrder(int order) {
        Component2D::setOrder(order);
        if (mTextComponent){
            mTextComponent->setOrder(order + 1);
        }
    }

    void Button::setPressedColor(glm::vec4 const &pressedColor) {
        Button::mPressedColor = pressedColor;
    }

    glm::vec4 const &Button::pressedColor() const {
        return mPressedColor;
    }

    void Button::setHoverColor(glm::vec4 const &hoverColor) {
        Button::mHoverColor = hoverColor;
    }

    glm::vec4 const &Button::hoverColor() const {
        return mHoverColor;
    }

    void Button::setNormalColor(glm::vec4 const &normalColor) {
        Button::mNormalColor = normalColor;
    }

    glm::vec4 const &Button::normalColor() const {
        return mNormalColor;
    }

    void Button::invokeClick() {
        onClick(this);
    }
}