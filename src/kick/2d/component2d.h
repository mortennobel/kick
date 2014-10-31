//
// Created by morten on 26/07/14.
//


#pragma once
#include "kick/scene/component.h"
#include "kick/scene/game_object.h"

namespace kick {
    class Panel;
    class Shader;

    class Component2D : public Component{
    public:
        Component2D(GameObject *gameObject);

        int getOrder() const;
        virtual void setOrder(int order);
        virtual Shader* getShader() const = 0;

        virtual void deactivated() override;

    protected:
        Panel* panel = nullptr;
        bool dirty = false;
    private:
        friend class Panel;
        int order = 0;
    };
}



