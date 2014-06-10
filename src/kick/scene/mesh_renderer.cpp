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

namespace kick {
    MeshRenderer::MeshRenderer(GameObject *gameObject)
    :ComponentRenderable{gameObject} {
    }
    
    MeshRenderer::~MeshRenderer(){
    }
    
    void MeshRenderer::activated(){
        transform = gameObject->getTransform();
    }
    
    void MeshRenderer::render(EngineUniforms *engineUniforms){
        for (int i=0;i<materials.size();i++){
            auto material = materials[i];
            auto shader = material->getShader();
            mesh->bind(shader);
            shader->bind_uniforms(material, engineUniforms, transform);
            mesh->render(i);
        }
    }
    
    void MeshRenderer::setMesh(Mesh *mesh){
        this->mesh = mesh;
    }
    
    Mesh* MeshRenderer::getMesh(){
        return mesh;
    }
    
    void MeshRenderer::setMaterials(const std::vector<Material *> &mats){
        this->materials = mats;
    }
    
    const std::vector<Material*> & MeshRenderer::getMaterials(){
        return materials;
    }
    
    void MeshRenderer::setMaterial(Material *material){
        if (materials.size()==0){
            materials.push_back(material);
        } else {
            materials[0] = material;
        }
    }
    
    Material* MeshRenderer::getMaterial(){
        if (materials.size()==0){
            return nullptr;
        }
        return materials[0];
    }
}