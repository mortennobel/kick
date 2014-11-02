//
// Created by Morten Nobel-Jørgensen on 31/10/14.
//


#pragma once

#include "kick/kick.h"

namespace kick {
    class Skybox : public ComponentRenderable {
        MeshRenderer*mMeshRenderer;
        Mesh*mMesh;
    public:
        Skybox(GameObject *gameObject);
        virtual ~Skybox();
        virtual void render(EngineUniforms *engineUniforms, Material *replacementMaterial);

        virtual int renderOrder();

        Material *material() const;

        void setMaterial(Material *material);
    };
}


