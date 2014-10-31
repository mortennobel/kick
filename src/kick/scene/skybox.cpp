//
// Created by Morten Nobel-Jørgensen on 31/10/14.
//

#include "skybox.h"
#include <iostream>

using namespace std;

kick::Skybox::Skybox(GameObject *gameObject)
        : ComponentRenderable(gameObject),
          meshRenderer{new MeshRenderer(gameObject)},
          mesh{new Mesh()}

{
    mesh->setMeshData(MeshFactory::createCubeData(2));
    meshRenderer->setMesh(mesh);
}

kick::Skybox::~Skybox() {
    delete meshRenderer;
}

void kick::Skybox::render(kick::EngineUniforms *engineUniforms, kick::Material *replacementMaterial) {
    if (!replacementMaterial){
        glDepthRange(0.99999f,1);
        meshRenderer->render(engineUniforms, nullptr);
        glDepthRange(0,1);  // default
    }
}

int kick::Skybox::getRenderOrder() {
    return 1999;
}

kick::Material *kick::Skybox::getMaterial() const {
    return meshRenderer->getMaterial();
}

void kick::Skybox::setMaterial(kick::Material *material) {
    meshRenderer->setMaterial(material);
}