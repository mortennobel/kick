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
#include "obj_data.h"

#include <string>

namespace kick {
    ObjData loadObjData(std::string objSource, std::function<std::string (std::string)> materialMap);
    ObjData loadObjData(std::string path, std::string filename);
    
    std::string fixPathEnd(std::string &path);
}