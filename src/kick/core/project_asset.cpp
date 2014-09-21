//
//  project_asset.cpp
//  KickCPP
//
//  Created by morten on 8/25/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/core/project_asset.h"
#include "kick/core/project.h"

namespace kick {
    ProjectAsset::ProjectAsset()
    {
        
    }
    
    ProjectAsset::~ProjectAsset(){
        
    }
    
    void ProjectAsset::destroy(){
        uid = 0;
    }
}