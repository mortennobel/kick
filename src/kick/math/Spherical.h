//
// Created by Morten Nobel-Jørgensen on 29/10/14.
//


#pragma once

#include "glm/glm.hpp"

namespace kick {


/// In mathematics, a spherical coordinate system is a coordinate system for
/// three-dimensional space where the position of a point is specified by three numbers:
/// the radial distance of that point from a fixed origin, its inclination angle measured
/// from a fixed zenith direction, and the azimuth angle of its orthogonal projection on
/// a reference plane that passes through the origin and is orthogonal to the zenith,
/// measured from a fixed reference direction on that plane.
///
/// The zenith direction is the up vector (0,1,0) and the azimuth is the right vector (1,0,0)
///
/// (From http://en.wikipedia.org/wiki/Spherical_coordinate_system )
    class Spherical {
    public:
        /// the radial distance of that point from a fixed origin.
        /// Radius must be >= 0
        float radius;

        /// azimuth angle of its orthogonal projection on
        /// a reference plane that passes through the origin and is orthogonal to the zenith

        float polar;

        /// <summary>
        /// elevation angle from the reference plane
        /// </summary>
        float elevation;

        glm::vec3 toCartesian();

        static Spherical cartesianToSpherical(glm::vec3 cartCoords);

        static glm::vec3 sphericalToCartesian(float radius, float polar, float elevation);
    };
}



