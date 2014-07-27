//
// Created by morten on 26/07/14.
//

#include "component2d.h"
#include "panel.h"
#include <iostream>

using namespace std;

namespace kick {

    int Component2D::getOrder() const {
        return order;
    }

    void Component2D::setOrder(int order) {
        if (Component2D::order != order) {
            Component2D::order = order;
            dirty = true;
            if (panel){
                panel->updateRenderOrder(this);
            }
        }
    }

    void Component2D::activated() {
        panel = gameObject->getComponentInParent<Panel>();
        if (panel){
            panel->registerComponent2D(this);
        }
    }

    void Component2D::deactivated() {
        if (panel) {
            panel->deregisterComponent2D(this);
        }
    }
}