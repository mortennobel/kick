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
        static std::shared_ptr<MeshData> createPointData();
        static std::shared_ptr<MeshData> createTriangleData();
        static std::shared_ptr<MeshData> createDiscData(unsigned short slices = 20);
        static std::shared_ptr<MeshData> createPlaneData();
        static std::shared_ptr<MeshData> createUVSphereData(unsigned short slices = 64,unsigned short stacks = 32,float radius = 1.0f);
        static std::shared_ptr<MeshData> createCubeData(float length = 1.0f);
        
    };
};