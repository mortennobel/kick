//
//  project_asset.h
//  KickCPP
//
//  Created by morten on 8/25/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once

#include <iostream>

namespace kick {
    class Project;
    
    class ProjectAsset {
    public:
        ProjectAsset();
        ~ProjectAsset();
        virtual void destroy();
        friend class Project;
    protected:

    private:
        ProjectAsset(const ProjectAsset&) = delete;
        int uid;
    };
}