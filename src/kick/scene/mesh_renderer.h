//
//  mesh_renderer.h
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 12/12/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once

#include "component.h"
#include "kick/material/material.h"
#include "component_renderable.h"
#include <vector>

namespace kick {
    class Transform;
    class Mesh;
    class GameObject;
    
    class MeshRenderer : public ComponentRenderable {
    public:
        MeshRenderer(GameObject *gameObject);
        ~MeshRenderer();

        virtual void render(EngineUniforms *engineUniforms, Material* replacementMaterial = nullptr);
        void setMesh(std::shared_ptr<Mesh> mesh);
        std::shared_ptr<Mesh> mesh();
        // set the first material
        void setMaterial(Material *material);
        // get first material (no nullptr if no material added)
        Material* material();

        Material* instancedMaterial();
        void setMaterials(const std::vector<Material *> &mats);
        const std::vector<Material*> &materials();
        const std::vector<Material*> &instancedMaterials();

        virtual int renderOrder();

    private:
        bool isInstanced = false;
        std::shared_ptr<Mesh> mMesh;
        std::shared_ptr<Transform> mTransform;
        std::vector<Material*> mMaterials;
    };
}