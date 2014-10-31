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
        setAnchor({0.5f,0.5f});
        textComponent = panel->createText(text);
        textComponent->getTransform()->setParent(getTransform());
        textComponent->setText(text);
        textComponent->setAnchor({0.5f,0.5f});
        textComponent->setOrder(getOrder()+1);
        updateTextureAndTxtColor();
    }

    void Button::setText(std::string const &text) {
        Button::text = text;
        if (textComponent){
            textComponent->setText(text);
        }
    }

    std::string Button::getText() const {
        return text;
    }

    void Button::setPressed(std::string const &pressed) {
        Button::pressedName = pressed;
        updateTextureAndTxtColor();
    }

    std::string Button::getPressed() const {
        return pressedName;
    }

    void Button::setHover(std::string const &hover) {
        Button::hoverName = hover;
        updateTextureAndTxtColor();
    }

    std::string Button::getHover() const {
        return hoverName;
    }

    void Button::setNormal(std::string const &normal) {
        Button::normalName = normal;
        updateTextureAndTxtColor();
    }

    std::string Button::getNormal() const {
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
        getGameObject()->getScene()->destroyGameObject(textComponent->getGameObject());
    }

    void Button::down(int button) {
        pressedButtons.insert(button);
        updateTextureAndTxtColor();
        onClick(this);
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
}