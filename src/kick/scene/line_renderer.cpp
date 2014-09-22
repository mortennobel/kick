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
        mesh = new Mesh();
        meshData = new MeshData();
        auto shader = Project::loadShader("assets/shaders/unlit.shader");
        material = new Material();
        material->setShader(shader);
        mesh->setMeshData(meshData);
    }

    LineRenderer::~LineRenderer() {
        Project::destroyAsset(mesh);
        Project::destroyAsset(meshData);
    }

    std::vector<glm::vec3> const &LineRenderer::getPoints() const {
        return points;
    }
    void LineRenderer::setPoints(std::vector<glm::vec3> const &points) {
        LineRenderer::points = points;
        rebuildMesh();
    }

    void LineRenderer::activated() {
        Component::activated();
        transform = gameObject->getTransform();
    }

    void LineRenderer::render(EngineUniforms *engineUniforms, Material* replacementMaterial) {
        auto mat = replacementMaterial?replacementMaterial : material;
        auto shader = mat->getShader().get();
        mesh->bind(shader);
        shader->bind_uniforms(mat, engineUniforms, transform);
        glLineWidth(width);
        mesh->render(0);
    }

    void LineRenderer::setMaterial(Material *material) {
        this->material = material;
    }

    Material *LineRenderer::getMaterial() {
        return material;
    }

    int LineRenderer::getRenderOrder() {
        return material->getRenderOrder();
    }

    void LineRenderer::rebuildMesh() {
        meshData->setPosition(points);

        vector<GLushort> idx;
        for (int i=0;i<points.size()-1;i++){
            idx.push_back(i);
            idx.push_back(i+1);
        }
        meshData->setSubmesh(0, idx, MeshType::Lines);
        mesh->setMeshData(meshData);
    }
}