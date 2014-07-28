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
    Button::Button(GameObject *gameObject) : Sprite(gameObject) {
        setAnchor({0.5f,0.5f});
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
        updateTexture();
    }

    std::string Button::getPressed() const {
        return pressedName;
    }

    void Button::setHover(std::string const &hover) {
        Button::hoverName = hover;
        updateTexture();
    }

    std::string Button::getHover() const {
        return hoverName;
    }

    void Button::setNormal(std::string const &normal) {
        Button::normalName = normal;
        updateTexture();
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

    void Button::updateTexture() {
        setSpriteName(currentSpriteName());
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

    void Button::activated() {
        Sprite::activated();
        textComponent = panel->createText(text);
        textComponent->getTransform()->setParent(getTransform());
        textComponent->setText(text);
        textComponent->setAnchor({0.5f,0.5f});
        textComponent->setOrder(getOrder()+1);
        updateTexture();
    }

    void Button::deactivated() {
        Sprite::deactivated();
        getGameObject()->getScene()->destroyGameObject(textComponent->getGameObject());
    }

    void Button::down(int button) {
        pressedButtons.insert(button);
        updateTexture();
        onClick(this);
    }

    void Button::pressed(int button) {

    }

    void Button::up(int button) {
        pressedButtons.erase(button);
        updateTexture();
    }

    void Button::over() {
        state = ButtonState::hover;
        updateTexture();
    }

    void Button::out() {
        state = ButtonState::normal;
        updateTexture();
    }

    void Button::setOrder(int order) {
        Component2D::setOrder(order);
        if (textComponent){
            textComponent->setOrder(order+1);
        }
    }
}