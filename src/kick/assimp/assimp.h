//
//  assimp.h
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 12/25/14.
//  Copyright (c) 2014 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once

#ifndef NO_ASSIMP


#include <string>
#include "kick/scene/scene.h"

namespace kick {

    struct AssImpSettings {
        //bool animations = true;
        bool textures = true;
        bool lights = true;
        bool cameras = true;
        bool meshes = true;
        bool materials = true;
    };

    class AssImp{
    public:
        static bool importData(std::string filename, Scene * scene, AssImpSettings importSettings = AssImpSettings{});
    };
}
#endif
