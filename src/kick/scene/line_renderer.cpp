//
// Created by Morten Nobel-Jørgensen on 25/08/14.
//

#include "line_renderer.h"
#include "kick/core/project.h"
#include "kick/mesh/mesh.h"
#include "log.h"

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

    void LineRenderer::setPoints(const std::vector<glm::vec3> &points, MeshType meshType, const std::vector<GLushort> &indices) {
        if (meshType != MeshType::Lines && meshType != MeshType::LineLoop && meshType != MeshType::LineStrip){
            logWarning("Invalid MeshType for line.");
        }
        LineRenderer::mMeshType = meshType;
        LineRenderer::mPoints = points;

        LineRenderer::mIndices = indices;

        rebuildMesh();
    }

    void LineRenderer::render(EngineUniforms *engineUniforms, Material* replacementMaterial) {
        if (mSmoothLine){
            glEnable(GL_LINE_SMOOTH);
        } else {
            glDisable(GL_LINE_SMOOTH);
        }
        auto mat = replacementMaterial?replacementMaterial : mMaterial;
        auto shader = mat->shader().get();
        mMesh->bind(shader);
        shader->bind_uniforms(mat, engineUniforms, mTransform);
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
        mMeshData->recomputeBounds();
        mMeshData->setSubmesh(0, mIndices, mMeshType);
        mMesh->setMeshData(mMeshData);
    }

    bool LineRenderer::smoothLine() const {
        return mSmoothLine;
    }

    void LineRenderer::setSmoothLine(bool smoothLine) {
        mSmoothLine = smoothLine;
    }

    std::vector<GLushort> const &LineRenderer::indices() const {
        return mIndices;
    }
}