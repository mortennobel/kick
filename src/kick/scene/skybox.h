//
// Created by Morten Nobel-Jørgensen on 31/10/14.
//


#pragma once

#include "kick/kick.h"

namespace kick {
    class Skybox : public ComponentRenderable {
        MeshRenderer* meshRenderer;
        Mesh* mesh;
    public:
        Skybox(GameObject *gameObject);
        virtual ~Skybox();
        virtual void render(EngineUniforms *engineUniforms, Material *replacementMaterial);

        virtual int getRenderOrder();

        Material * getMaterial() const;

        void setMaterial(Material *material);
    };
}


