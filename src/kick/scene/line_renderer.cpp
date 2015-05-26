//
// Created by Morten Nobel-Jørgensen on 25/08/14.
//

#include "line_renderer.h"
#include "kick/core/project.h"
#include "kick/mesh/mesh.h"
#include "kick/core/debug.h"

using namespace std;
using namespace glm;

namespace kick {
    LineRenderer::LineRenderer(GameObject *gameObject) : ComponentRenderable(gameObject) {
        mMesh = make_shared<Mesh>();
        auto meshData = make_shared<MeshData>();
        auto shader = Project::loadShader("assets/shaders/unlit_line.shader");

        mMaterial = new Material();
        mMaterial->setShader(shader);
        mMesh->setMeshData(meshData);
        mTransform = gameObject->transform();
    }

    LineRenderer::~LineRenderer() {
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
        if (!enabled()){
            return;
        }
#ifndef KICK_CONTEXT_ES2
        if (mSmoothLine){
            glEnable(GL_LINE_SMOOTH);
        } else {
            glDisable(GL_LINE_SMOOTH);
        }
#endif
        auto mat = replacementMaterial?replacementMaterial : mMaterial;
        auto shader = mat->shader().get();
        if (mPoints.size()){
            mMesh->bind(shader);
            shader->bind_uniforms(mat, engineUniforms, mTransform.get());
            mMesh->render(0);
        }
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
        auto meshData = mMesh->meshData();
        meshData->setPosition(mPoints);
        meshData->recomputeBounds();
        meshData->setSubmesh(0, mIndices, mMeshType);
        mMesh->setMeshData(meshData);
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