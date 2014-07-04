//
//  ObjSave.h
//  KickObjLoader
//
//  Created by morten on 9/1/13.
//  Copyright (c) 2013 morten. All rights reserved.
//

#pragma once

#include <iostream>
#include <functional>
#include "obj_data.h"

#include <string>

namespace kick {
    void saveObjData(ObjData & data, std::string path, std::string filename);
}