//
//  mesh.h
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 07/12/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include "kick/core/project_asset.h"
#include "kick/math/aabb.h"
#include "kick/material/shader.h"
#include "kick/core/kickgl.h"
#include "kick/mesh/mesh_data.h"

namespace kick {
    /**
     * Represents Mesh data on the GPU. Whenever mesh data is updated, then MeshData needs to
     * be reassigned (future update would be using observer pattern to make this update 
     * automatically).
     */
    class Mesh : public ProjectAsset {
    public:
        Mesh();
        ~Mesh();
        void bind(Shader * shader);
        void render(unsigned int submeshIndex);
        std::string name();
        void setName(std::string n);
        void setMeshData(std::shared_ptr<MeshData> m);
        std::shared_ptr<MeshData> meshData();
    private:
        void updateArrayBufferStructure(Shader *shader);
        void updateMeshData(MeshData *mesh_data);
        std::unordered_map<Shader*, GLuint> mVertexArrayObject;
        std::vector<InterleavedRecord> mInterleavedFormat;
        std::string mName;
        std::shared_ptr<MeshData> mMeshData;
        std::vector<SubMeshData> mSubmeshData;
        GLuint mVertexBufferId;
        GLuint mElementBufferId;
    };
};