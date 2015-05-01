//
// Created by morten on 26/07/14.
//

#include "component2d.h"
#include "canvas.h"
#include <iostream>

using namespace std;

namespace kick {

    Component2D::Component2D(GameObject *gameObject, std::shared_ptr<Canvas> canvas) : Component(gameObject), mCanvas(canvas) {
    }

    int Component2D::order() const {
        return mOrder;
    }

    void Component2D::setOrder(int order) {
        if (Component2D::mOrder != order) {
            Component2D::mOrder = order;
            if (mCanvas){
                mCanvas->updateRenderOrder(dynamic_pointer_cast<Component2D>(shared_from_this()));
            }
        }
    }

    void Component2D::deactivated() {
        if (mCanvas) {
            mCanvas->deregisterComponent2D(dynamic_pointer_cast<Component2D>(shared_from_this()));
        }
    }
}