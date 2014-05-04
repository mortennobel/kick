//
//  resource_loader.h
//  KickCPP
//
//  Created by morten on 9/9/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once

#include <iostream>

namespace kick {
    class ProjectAsset;

    class AssetLoader {
    public:
        virtual ProjectAsset *loadAsset(int assetId);
    };
}