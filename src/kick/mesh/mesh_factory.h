//
//  mesh_factory.h
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 05/12/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once

#include <memory>
#include "kick/mesh/mesh_data.h"

namespace kick {
    class MeshFactory {
    public:
        static MeshData *createPointData();
        static MeshData *createTriangleData();
        static MeshData *createDiscData(unsigned short slices = 20);
        static MeshData *createPlaneData();
        static MeshData *createUVSphereData(unsigned short slices = 64,unsigned short stacks = 32,float radius = 1.0f);
        static MeshData *createCubeData(float length = 1.0f);
        
    };
};