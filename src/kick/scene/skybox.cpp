//
// Created by Morten Nobel-Jørgensen on 31/10/14.
//

#include "skybox.h"
#include <iostream>

using namespace std;

kick::Skybox::Skybox(GameObject *gameObject)
        : ComponentRenderable(gameObject),
          mMeshRenderer{new MeshRenderer(gameObject)},
          mMesh{new Mesh()}

{
    mMesh->setMeshData(MeshFactory::createCubeData(2));
    mMeshRenderer->setMesh(mMesh);
}

kick::Skybox::~Skybox() {
    delete mMeshRenderer;
}

void kick::Skybox::render(kick::EngineUniforms *engineUniforms, kick::Material *replacementMaterial) {
    if (!replacementMaterial){
        glDepthRange(0.99999f,1);
        mMeshRenderer->render(engineUniforms, nullptr);
        glDepthRange(0,1);  // default
    }
}

int kick::Skybox::renderOrder() {
    return 1999;
}

kick::Material *kick::Skybox::material() const {
    return mMeshRenderer->material();
}

void kick::Skybox::setMaterial(kick::Material *material) {
    mMeshRenderer->setMaterial(material);
}