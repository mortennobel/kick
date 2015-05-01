//
// Created by morten on 26/07/14.
//


#pragma once
#include "kick/scene/component.h"
#include "kick/scene/game_object.h"
#include "kick/math/bounds2.h"

namespace kick {
    class Canvas;
    class Shader;

    class Component2D : public Component {
    public:
        Component2D(GameObject *gameObject, std::shared_ptr<Canvas> canvas);

        int order() const;
        virtual void setOrder(int order);
        virtual Shader* shader() const = 0;

        virtual void deactivated() override;
        virtual Bounds2 bounds() const = 0;
        virtual void setBounds(Bounds2 bounds) = 0;
    protected:
        std::shared_ptr<Canvas> mCanvas;
    private:
        friend class Canvas;
        int mOrder = 0;
    };
}



