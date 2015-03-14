//
// Created by morten on 19/07/14.
//

#include "button.h"
#include "kick/scene/scene.h"
#include "kick/scene/transform.h"
#include "panel.h"
#include <iostream>

using namespace std;

namespace kick {
    Button::Button(GameObject *gameObject)
            : Sprite{gameObject},
              onClick{[](Button*){}}
    {
        setType(SpriteType::Sliced);
        setAnchor({0.5f,0.5f});
        textComponent = panel->createLabel(mText);
        textComponent->transform()->setParent(transform());
        textComponent->setText(mText);
        textComponent->setAnchor({0.5f,0.5f});
        textComponent->setOrder(getOrder()+1);
        updateTextureAndTxtColor();
    }

    void Button::setText(std::string const &text) {
        Button::mText = text;
        if (textComponent){
            textComponent->setText(text);
        }
    }

    std::string Button::text() const {
        return mText;
    }

    void Button::setPressedSprite(std::string const &pressed) {
        Button::pressedName = pressed;
        updateTextureAndTxtColor();
    }

    std::string Button::pressedSprite() const {
        return pressedName;
    }

    void Button::setHoverSprite(std::string const &hover) {
        Button::hoverName = hover;
        updateTextureAndTxtColor();
    }

    std::string Button::hoverSprite() const {
        return hoverName;
    }

    void Button::setNormalSprite(std::string const &normal) {
        Button::normalName = normal;
        updateTextureAndTxtColor();
    }

    std::string Button::normalSprite() const {
        return normalName;
    }

    void Button::setOnClick(std::function<void(Button*)> const &onClick) {
        Button::onClick = onClick;
    }

    std::function<void(Button*)> const &Button::getOnClick() const {
        return onClick;
    }

    void Button::updateTextureAndTxtColor() {
        setSpriteName(currentSpriteName());
        if (pressedButtons.size()>0){
            textComponent->getMaterial()->setUniform("mainColor", pressedColor);
        } else {
            if (state == ButtonState::normal){
                textComponent->getMaterial()->setUniform("mainColor", normalColor);
            } else {
                textComponent->getMaterial()->setUniform("mainColor", hoverColor);
            }
        }
    }

    std::string Button::currentSpriteName() {
        if (pressedButtons.size()>0){
            return pressedName;
        }
        switch (state){
            case ButtonState::normal:
                return normalName;
            case ButtonState::hover:
                return hoverName;
            default: ////case ButtonState::pressed:
                return pressedName;
        }
    }



    void Button::deactivated() {
        Sprite::deactivated();
        gameObject()->scene()->destroyGameObject(textComponent->gameObject());
    }

    void Button::down(int button) {
        pressedButtons.insert(button);
        updateTextureAndTxtColor();
        invokeClick();
    }

    void Button::pressed(int button) {

    }

    void Button::up(int button) {
        pressedButtons.erase(button);
        updateTextureAndTxtColor();
    }

    void Button::over() {
        state = ButtonState::hover;
        updateTextureAndTxtColor();
    }

    void Button::out() {
        state = ButtonState::normal;
        updateTextureAndTxtColor();
    }

    void Button::setOrder(int order) {
        Component2D::setOrder(order);
        if (textComponent){
            textComponent->setOrder(order+1);
        }
    }

    void Button::setPressedColor(glm::vec4 const &pressedColor) {
        Button::pressedColor = pressedColor;
    }

    glm::vec4 const &Button::getPressedColor() const {
        return pressedColor;
    }

    void Button::setHoverColor(glm::vec4 const &hoverColor) {
        Button::hoverColor = hoverColor;
    }

    glm::vec4 const &Button::getHoverColor() const {
        return hoverColor;
    }

    void Button::setNormalColor(glm::vec4 const &normalColor) {
        Button::normalColor = normalColor;
    }

    glm::vec4 const &Button::getNormalColor() const {
        return normalColor;
    }

    void Button::invokeClick() {
        onClick(this);
    }
}