//
// Created by morten on 30/06/14.
//


#include "kick/scene/component.h"
#include "kick/scene/game_object.h"

#include "glm/glm.hpp"
#pragma once

namespace kick {
    class Sprite : public Component{
    public:
        Sprite(GameObject *gameObject) : Component(gameObject) {
        }

        glm::vec3 const &getPos() const {
            return pos;
        }

        void setPos(glm::vec3 const &pos) {
            Sprite::pos = pos;
        }

        float getWidth() const {
            return width;
        }

        void setWidth(float width) {
            Sprite::width = width;
        }

        float getHeight() const {
            return height;
        }

        void setHeight(float height) {
            Sprite::height = height;
        }
    private:
        glm::vec3 pos;
        float width;
        float height;
    };
}



