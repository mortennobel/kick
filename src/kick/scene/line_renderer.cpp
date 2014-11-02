//
// Created by Morten Nobel-Jørgensen on 25/08/14.
//

#include "line_renderer.h"
#include "kick/core/project.h"
#include "kick/mesh/mesh.h"

using namespace std;
using namespace glm;

namespace kick {
    LineRenderer::LineRenderer(GameObject *gameObject) : ComponentRenderable(gameObject) {
        mMesh = new Mesh();
        mMeshData = new MeshData();
        auto shader = Project::loadShader("assets/shaders/unlit.shader");
        mMaterial = new Material();
        mMaterial->setShader(shader);
        mMesh->setMeshData(mMeshData);
        mTransform = gameObject->transform();
    }

    LineRenderer::~LineRenderer() {
        Project::destroyAsset(mMesh);
        Project::destroyAsset(mMeshData);
    }

    std::vector<glm::vec3> const &LineRenderer::points() const {
        return mPoints;
    }
    void LineRenderer::setPoints(std::vector<glm::vec3> const &points) {
        LineRenderer::mPoints = points;
        rebuildMesh();
    }

    void LineRenderer::render(EngineUniforms *engineUniforms, Material* replacementMaterial) {
        auto mat = replacementMaterial?replacementMaterial : mMaterial;
        auto shader = mat->shader().get();
        mMesh->bind(shader);
        shader->bind_uniforms(mat, engineUniforms, mTransform);
        glLineWidth(mWidth);
        mMesh->render(0);
    }

    void LineRenderer::setMaterial(Material *material) {
        this->mMaterial = material;
    }

    Material *LineRenderer::material() {
        return mMaterial;
    }

    int LineRenderer::renderOrder() {
        return mMaterial->renderOrder();
    }

    void LineRenderer::rebuildMesh() {
        mMeshData->setPosition(mPoints);

        vector<GLushort> idx;
        for (int i=0;i< mPoints.size()-1;i++){
            idx.push_back(i);
            idx.push_back(i+1);
        }
        mMeshData->setSubmesh(0, idx, MeshType::Lines);
        mMesh->setMeshData(mMeshData);
    }
}