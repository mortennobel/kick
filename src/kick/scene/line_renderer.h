//
// Created by Morten Nobel-Jørgensen on 25/08/14.
//



#pragma once

#include "component.h"
#include "kick/material/material.h"
#include "component_renderable.h"
#include <vector>

namespace kick {
    class Transform;
    class Mesh;
    class GameObject;

    class LineRenderer : public ComponentRenderable {
    public:
        LineRenderer(GameObject *gameObject);
        ~LineRenderer();
        virtual void activated();
        virtual void render(EngineUniforms *engineUniforms, Material* replacementMaterial = nullptr) override;

        std::vector<glm::vec3> const & getPoints() const;
        void setPoints(std::vector<glm::vec3> const &points);

        // set the material
        void setMaterial(Material *material);
        // get material
        Material*getMaterial();

        virtual int getRenderOrder();
    private:
        void rebuildMesh();
        std::vector<glm::vec3> points;
        Mesh* mesh;
        Transform* transform;
        Material* material;
        MeshData* meshData;
        float width = 10.1f;
    };
}



