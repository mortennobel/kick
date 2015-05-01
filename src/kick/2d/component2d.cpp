//
// Created by morten on 26/07/14.
//

#include "component2d.h"
#include "canvas.h"
#include <iostream>

using namespace std;

namespace kick {

    Component2D::Component2D(GameObject *gameObject) : Component(gameObject) {
        mPanel = gameObject->componentInParent<Canvas>();
    }

    int Component2D::order() const {
        return mOrder;
    }

    void Component2D::setOrder(int order) {
        if (Component2D::mOrder != order) {
            Component2D::mOrder = order;
            if (mPanel){
                mPanel->updateRenderOrder(dynamic_pointer_cast<Component2D>(shared_from_this()));
            }
        }
    }

    void Component2D::deactivated() {
        if (mPanel) {
            mPanel->deregisterComponent2D(dynamic_pointer_cast<Component2D>(shared_from_this()));
        }
    }
}