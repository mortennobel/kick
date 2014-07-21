//
// Created by morten on 19/07/14.
//

#include "button.h"
#include "kick/scene/scene.h"
#include "kick/scene/transform.h"

namespace kick {
    void Button::setText(std::string const &text) {
        Button::text = text;
    }

    std::string Button::getText() const {
        return text;
    }

    void Button::setPressed(std::string const &pressed) {
        Button::pressed = pressed;
        updateTexture();
    }

    std::string Button::getPressed() const {
        return pressed;
    }

    void Button::setHover(std::string const &hover) {
        Button::hover = hover;
        updateTexture();
    }

    std::string Button::getHover() const {
        return hover;
    }

    void Button::setNormal(std::string const &normal) {
        Button::normal = normal;
        updateTexture();
    }

    std::string Button::getNormal() const {
        return normal;
    }

    void Button::setOnClick(std::function<void(Button*)> const &onClick) {
        Button::onClick = onClick;
    }

    std::function<void(Button*)> const &Button::getOnClick() const {
        return onClick;
    }

    void Button::updateTexture() {
        if (!sprite){
            return;
        }
        switch (state){
            case ButtonState::normal:
                sprite->setSpriteName(normal);
                break;
            case ButtonState::hover:
                sprite->setSpriteName(hover);
                break;
            case ButtonState::pressed:
                sprite->setSpriteName(pressed);
                break;

        }
    }

    void Button::activated() {
        sprite = getGameObject()->addComponent<Sprite>();
        sprite->setAnchor({0.5f,0.5f});
        textComponent = getGameObject()->getScene()->createText(text);
        textComponent->getTransform()->setParent(getTransform());
        if (textureAtlas){
            sprite->setTextureAtlas(textureAtlas);
        }
        updateTexture();
    }

    void Button::deactivated() {
        getGameObject()->destroyComponent(sprite);
        getGameObject()->getScene()->destroyGameObject(textComponent->getGameObject());
    }

    void Button::update() {

    }

    void Button::setTextureAtlas(std::shared_ptr<TextureAtlas> textureAtlas) {
        Button::textureAtlas = textureAtlas;
        if (sprite){
            sprite->setTextureAtlas(textureAtlas);
        }
    }

    std::shared_ptr<TextureAtlas> Button::getTextureAtlas() const {
        return textureAtlas;
    }
}