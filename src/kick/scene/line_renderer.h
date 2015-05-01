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

        virtual void render(EngineUniforms *engineUniforms, Material* replacementMaterial = nullptr) override;

        std::vector<glm::vec3> const &points() const;
        std::vector<GLushort> const & indices() const;

        // meshType should be MeshType::Lines, MeshType::LineLoop, MeshType::LineStrip
        void setPoints(const std::vector<glm::vec3>  &points, MeshType meshType = MeshType::Lines, const std::vector<GLushort> &indices = std::vector<GLushort>{});

        // set the material
        void setMaterial(Material *material);
        // get material
        Material*material();

        virtual int renderOrder();

        bool smoothLine() const;

        void setSmoothLine(bool smoothLine);

    private:
        void rebuildMesh();
        MeshType mMeshType;
        std::vector<glm::vec3> mPoints;
        std::vector<GLushort> mIndices;
        std::shared_ptr<Mesh> mMesh;
        std::shared_ptr<Transform> mTransform;
        Material* mMaterial;
        bool mSmoothLine = true;
    };
}



