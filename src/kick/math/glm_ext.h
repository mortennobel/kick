//
//  math.h
//  KickCPP
//
//  Created by morten on 8/13/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace kick {

    glm::mat4 TRS(glm::vec3 translate, glm::quat rotate, glm::vec3 scale);

    glm::mat4 TRSInverse(glm::vec3 translate, glm::quat rotate, glm::vec3 scale);

    glm::quat lookAt(glm::vec3 eyePosition, glm::vec3 lookAtPos, glm::vec3 up = glm::vec3(0,1,0));

};
