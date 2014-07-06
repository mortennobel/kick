//
//  mesh_factory.cpp
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 05/12/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/mesh/mesh_factory.h"
#include"kick/core/cpp_ext.h"
#include"kick/core/project.h"
#include "glm/gtx/string_cast.hpp"

using namespace std;
using namespace glm;

namespace kick {
    MeshData* MeshFactory::createPointData(){
        auto meshData = Project::createAsset<MeshData>();
        meshData->setPosition({vec3{0,0,0}});
        meshData->setSubmesh(0, {0}, MeshType::Points);
        return meshData;
    }
    
    MeshData* MeshFactory::createTriangleData(){
        auto meshData = Project::createAsset<MeshData>();
        float sqrt75 = glm::sqrt(0.75f);
        meshData->setPosition({
            vec3{0,1,0},
            vec3{-sqrt75,-0.5f,0},
            vec3{sqrt75,-0.5f,0}
        });
        meshData->setTexCoord0({
            vec2{0.0f,1},
            vec2{0.125f,0.25f},
            vec2{1- 0.125f, 0.25f}
        });
        meshData->setNormal({
            vec3{0,0,1},
            vec3{0,0,1},
            vec3{0,0,1}
        });
        meshData->setColor({
            vec4{1,0,0,1},
            vec4{0,1,0,1},
            vec4{0,0,1,1}
        });
        meshData->setSubmesh(0, {0,1,2}, MeshType::Triangles);
        return meshData;
    }
    
    MeshData *MeshFactory::createDiscData(unsigned short slices){
        auto meshData = Project::createAsset<MeshData>();
        size_t vertexCount = slices+1;
        vector<vec3> vertices{vertexCount};
        vector<vec2> uvs{vertexCount};
        vector<vec3> normals{vertexCount};
        vector<vec4> colors{vertexCount};
        vector<GLushort> indices(vertexCount);
        for (unsigned short i = 0; i <= slices; i++) {
            auto fraction = 2 * M_PI * i / slices;
            vertices[i] = vec3{sin(fraction),
                                -cos(fraction),
                                0};
            uvs[i] = vec2{sin(fraction) * 0.5 + 0.5 , -cos(fraction) * 0.5 + 0.5};
            colors[i] = vec4{sin(fraction) * 0.5 + 0.5 , -cos(fraction) * 0.5 + 0.5,0,1};
            normals[i] = vec3{0};
            indices[i] = i;
        }
        meshData->setPosition(vertices);
        meshData->setTexCoord0(uvs);
        meshData->setNormal(normals);
        meshData->setColor(colors);
        meshData->setSubmesh(0, indices, MeshType::TriangleFan);
        return meshData;
    }
    
    MeshData *MeshFactory::createPlaneData(){
        auto meshData = Project::createAsset<MeshData>();
        
        meshData->setPosition({
            vec3{1, -1, 0},
            vec3{1, 1, 0},
            vec3{-1, -1, 0},
            vec3{-1, 1, 0}
        });
        meshData->setTexCoord0({
            vec2{1, 0},
            vec2{1, 1},
            vec2{0, 0},
            vec2{0, 1}
        });
        meshData->setNormal({
            vec3{0, 0, 1},
            vec3{0, 0, 1},
            vec3{0, 0, 1},
            vec3{0, 0, 1}
        });
        meshData->setColor({
            vec4{1, 0, 0, 1},
            vec4{0, 1, 0, 1},
            vec4{0, 0, 1, 1},
            vec4{1, 1, 1, 1}
        });
        meshData->setSubmesh(0, {0, 1, 2, 2, 1, 3}, MeshType::Triangles);
        
        return meshData;
    }
    
    MeshData * MeshFactory::createUVSphereData(unsigned short slices, unsigned short stacks, float radius){
        auto meshData = Project::createAsset<MeshData>();
        size_t vertexCount = (stacks+1) * (slices + 1);
        vector<vec3> vertices{vertexCount};
        vector<vec2> uvs{vertexCount};
        vector<vec3> normals{vertexCount};
        vector<vec4> colors{vertexCount};
        vector<GLushort> indices;

        int index = 0;
        // create vertices
        for (unsigned short j = 0; j <= stacks; j++) {
            float latitude1 = (M_PI / stacks) * j - (M_PI / 2);
            float sinLat1 = sin(latitude1);
            float cosLat1 = cos(latitude1);
            for (int i = 0; i <= slices; i++) {
                float longitude = ((M_PI * 2) / slices) * i;
                float sinLong = sin(longitude);
                float cosLong = cos(longitude);
                vec3 normal{cosLong * cosLat1,
                    sinLat1,
                    sinLong * cosLat1};
                normal = normalize(normal);
                normals[index] = normal;
                uvs[index] = vec2{1 - i /(float) slices, j /(float) stacks};
                vertices[index] = normal * radius;
                colors[index] = vec4(normal*0.5f+vec3(0.5f), 1.0f);
                index++;
            }
        }
        // create indices
        for (int j = 0; j < stacks; j++) {
            if (j > 0) {
                indices.push_back(j * (slices + 1)); // make degenerate
            }
            for (int i = 0; i <= slices; i++) {
                index = j * (slices + 1) + i;
                indices.push_back(index);
                indices.push_back(index + slices + 1);
            }
            if (j + 1 < stacks) {
                indices.push_back(index + slices + 1); // make degenerate
            }
        }
        
        meshData->setPosition(vertices);
        meshData->setTexCoord0(uvs);
        meshData->setNormal(normals);
        meshData->setColor(colors);
        meshData->setSubmesh(0, indices, MeshType::TriangleStrip);
        
        return meshData;
    }
    
    MeshData * MeshFactory::createCubeData(float length){
        auto meshData = Project::createAsset<MeshData>();
        //    v6----- v5
        //   /|      /|
        //  v1------v0|
        //  | |     | |
        //  | |v7---|-|v4
        //  |/      |/
        //  v2------v3
        meshData->setPosition({
            vec3{length, length, length},
            vec3{-length, length, length},
            vec3{-length, -length, length},
            vec3{length, -length, length},     // v0-v1-v2-v3
            vec3{length, length, length},
            vec3{length, -length, length},
            vec3{length, -length, -length},
            vec3{length, length, -length},     // v0-v3-v4-v5
            vec3{length, length, length},
            vec3{length, length, -length},
            vec3{-length, length, -length},
            vec3{-length, length, length},     // v0-v5-v6-v1
            vec3{-length, length, length},
            vec3{-length, length, -length},
            vec3{-length, -length, -length},
            vec3{-length, -length, length},    // v1-v6-v7-v2
            vec3{-length, -length, -length},
            vec3{length, -length, -length},
            vec3{length, -length, length},
            vec3{-length, -length, length},    // v7-v4-v3-v2
            vec3{length, -length, -length},
            vec3{-length, -length, -length},
            vec3{-length, length, -length},
            vec3{length, length, -length}      // v4-v7-v6-v5
        });
        meshData->setTexCoord0({
            vec2{1, 1},
            vec2{0, 1},
            vec2{0, 0},
            vec2{1, 0},                    // v0-v1-v2-v3
            vec2{0, 1},
            vec2{0, 0},
            vec2{1, 0},
            vec2{1, 1},              // v0-v3-v4-v5
            vec2{1, 0},
            vec2{1, 1},
            vec2{0, 1},
            vec2{0, 0},              // v0-v5-v6-v1 (top)
            vec2{1, 1},
            vec2{0, 1},
            vec2{0, 0},
            vec2{1, 0},              // v1-v6-v7-v2
            vec2{1, 1},
            vec2{0, 1},
            vec2{0, 0},
            vec2{1, 0},              // v7-v4-v3-v2 (bottom)
            vec2{0, 0},
            vec2{1, 0},
            vec2{1, 1},
            vec2{0, 1}             // v4-v7-v6-v5
        });
        meshData->setNormal({
            vec3{0, 0, 1},
            vec3{0, 0, 1},
            vec3{0, 0, 1},
            vec3{0, 0, 1},             // v0-v1-v2-v3
            vec3{1, 0, 0},
            vec3{1, 0, 0},
            vec3{1, 0, 0},
            vec3{1, 0, 0},              // v0-v3-v4-v5
            vec3{0, 1, 0},
            vec3{0, 1, 0},
            vec3{0, 1, 0},
            vec3{0, 1, 0},              // v0-v5-v6-v1
            vec3{-1, 0, 0},
            vec3{-1, 0, 0},
            vec3{-1, 0, 0},
            vec3{-1, 0, 0},          // v1-v6-v7-v2
            vec3{0, -1, 0},
            vec3{0, -1, 0},
            vec3{0, -1, 0},
            vec3{0, -1, 0},         // v7-v4-v3-v2
            vec3{0, 0, -1},
            vec3{0, 0, -1},
            vec3{0, 0, -1},
            vec3{0, 0, -1}        // v4-v7-v6-v5
        });
        meshData->setColor({
            vec4{1, 1, 1, 1},
            vec4{1, 1, 0, 1},
            vec4{1, 0, 0, 1},
            vec4{1, 0, 1, 1},              // v0-v1-v2-v3
            vec4{1, 1, 1, 1},
            vec4{1, 0, 1, 1},
            vec4{0, 0, 1, 1},
            vec4{0, 1, 1, 1},              // v0-v3-v4-v5
            vec4{1, 1, 1, 1},
            vec4{0, 1, 1, 1},
            vec4{0, 1, 0, 1},
            vec4{1, 1, 0, 1},              // v0-v5-v6-v1
            vec4{1, 1, 0, 1},
            vec4{0, 1, 0, 1},
            vec4{0, 0, 0, 1},
            vec4{1, 0, 0, 1},              // v1-v6-v7-v2
            vec4{0, 0, 0, 1},
            vec4{0, 0, 1, 1},
            vec4{1, 0, 1, 1},
            vec4{1, 0, 0, 1},              // v7-v4-v3-v2
            vec4{0, 0, 1, 1},
            vec4{0, 0, 0, 1},
            vec4{0, 1, 0, 1},
            vec4{0, 1, 1, 1}             // v4-v7-v6-v5
        });
        meshData->setSubmesh(0, {
            0, 1, 2,
            0, 2, 3,
            4, 5, 6,
            4, 6, 7,
            8, 9, 10,
            8, 10, 11,
            12, 13, 14,
            12, 14, 15,
            16, 17, 18,
            16, 18, 19,
            20, 21, 22,
            20, 22, 23},
            MeshType::Triangles);
        return meshData;
    }
}
