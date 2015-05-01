//
//  mesh_renderer.cpp
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 12/12/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/scene/mesh_renderer.h"
#include "kick/scene/game_object.h"
#include "kick/mesh/mesh.h"
#include "kick/scene/game_object.h"
#include "kick/core/debug.h"

namespace kick {
    MeshRenderer::MeshRenderer(GameObject *gameObject)
    :ComponentRenderable{gameObject} {
        mTransform = gameObject->transform();
    }
    
    MeshRenderer::~MeshRenderer(){
    }

    void MeshRenderer::render(EngineUniforms *engineUniforms, Material* replacementMaterial){
        if (!enabled()){
            return;
        }
        if (mMesh == nullptr){
            logWarning("Cannot render mesh is null");
        }
        for (unsigned int i=0;i< mMaterials.size();i++){
            auto material = replacementMaterial ? replacementMaterial : mMaterials[i];
            auto shader = material->shader().get();
            mMesh->bind(shader);
            shader->bind_uniforms(material, engineUniforms, mTransform.get());
            mMesh->render(i);
        }
    }
    
    void MeshRenderer::setMesh(std::shared_ptr<Mesh> mesh){
        this->mMesh = mesh;
    }

    std::shared_ptr<Mesh>  MeshRenderer::mesh(){
        return mMesh;
    }
    
    void MeshRenderer::setMaterials(const std::vector<Material *> &mats){
        isInstanced = false;
        this->mMaterials = mats;
    }
    
    const std::vector<Material*> & MeshRenderer::materials(){
        return mMaterials;
    }
    
    void MeshRenderer::setMaterial(Material *material){
        isInstanced = false;
        if (mMaterials.size()==0){
            mMaterials.push_back(material);
        } else {
            mMaterials[0] = material;
        }
    }
    
    Material* MeshRenderer::material(){
        if (mMaterials.size()==0){
            return nullptr;
        }
        return mMaterials[0];
    }

    int MeshRenderer::renderOrder() {
        if (mMaterials.size() == 0){
            return 0;
        }
        return mMaterials[0]->renderOrder();
    }

    Material *MeshRenderer::instancedMaterial() {
        if (!isInstanced){
            isInstanced = true;
            for (int i=0;i<mMaterials.size();i++){
                mMaterials[i] = new Material(*mMaterials[i]); // memory leak
            }
        }
        return material();
    }

    const std::vector<Material *> &MeshRenderer::instancedMaterials() {
        instancedMaterial();
        return materials();
    }
}