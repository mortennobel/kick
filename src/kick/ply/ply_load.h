//
//  ObjLoader.h
//  KickObjLoader
//
//  Created by morten on 8/31/13.
//  Copyright (c) 2013 morten. All rights reserved.
//

#pragma once

#include <iostream>
#include <functional>
#include "kick/mesh/mesh_data.h"

#include <string>

namespace kick {
    MeshData *loadPlyData(std::string objSource);
    MeshData *loadPlyData(std::string path, std::string filename);
}