//
// Created by Morten Nobel-Jørgensen on 29/10/14.
//

#include "spherical_.h"

namespace kick{

glm::vec3 Spherical::toCartesian() {
    return sphericalToCartesian(radius, polar, elevation);
}

Spherical Spherical::cartesianToSpherical(glm::vec3 cartCoords) {
    if (cartCoords.x == 0)
        cartCoords.x = FLT_EPSILON;
    Spherical res;
    res.radius = (float) sqrt((cartCoords.x * cartCoords.x)
                + (cartCoords.y * cartCoords.y)
                + (cartCoords.z * cartCoords.z));
    res.polar = (float) atan(cartCoords.z / cartCoords.x);
    if (cartCoords.x < 0)
        res.polar += M_PI;
    res.elevation = (float) asin(cartCoords.y / res.radius);
    return res;
}

glm::vec3 Spherical::sphericalToCartesian(float radius, float polar, float elevation) {
    float a = (float) (radius * cos(elevation));
    glm::vec3 outCart;
    outCart.x = (float) (a * cos(polar));
    outCart.y = (float) (radius * sin(elevation));
    outCart.z = (float) (a * sin(polar));
    return outCart;
}

}