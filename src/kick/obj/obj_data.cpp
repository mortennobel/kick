//
//  ObjData.cpp
//  KickObjLoader
//
//  Created by morten on 8/31/13.
//  Copyright (c) 2013 morten. All rights reserved.
//

#include "kick/obj/obj_data.h"
#include <stdexcept>
#include <string>
#include "kick/core/debug.h"
#include "kick/core/project.h"

using namespace std;

namespace kick {
    bool ObjData::verifyFaces() {
        for (auto & face : faces) {
            if (face.size()<3){
//                throw length_error();
                logError("Faces must be at least 3 vertices");
                return false;
            }
            
            for (auto & vertex : face) {
                if (vertex.vertexPositionIdx > vertexPositions.size()){
                    logError(string{"Invalid vertex position index. Index was "} +
                            std::to_string(vertex.vertexPositionIdx) +
                            " number of vertex positions is "+
                            std::to_string(vertexPositions.size()));
                    return false;
                }
                if (vertex.vertexPositionIdx<=0){
                    logError(string{"Invalid vertex position index. Must be positive. Was "} +
                            std::to_string(vertex.vertexPositionIdx));
                    return false;
                }
                
                if (vertex.textureIdx > textureCoords.size()){
                    logError(string{"Invalid texture index. Index was "} +
                            std::to_string(vertex.textureIdx) +
                            " number of texture indices is "+
                            std::to_string(textureCoords.size()));
                    return false;
                }
                
                if (vertex.normalIdx > normals.size()){
                    logError(string{"Invalid normal index. Index was "} +
                            std::to_string(vertex.normalIdx) +
                            " number of normals is "+
                            std::to_string(normals.size()));
                    return false;
                }
            }
        }
        
        return true;
    }
    
    ObjMaterial* ObjData::material(std::string name){
        for (auto & mat : materials){
            if (mat.name == name){
                return &mat;
            }
        }
        return nullptr;
    }

    std::vector<unsigned int> ObjData::indices() {
        std::vector<unsigned int> res;
        for (auto & f : faces){
            assert (f.size()==3);
            for (int i=0;i<3;i++){
                res.push_back((unsigned short)(f[i].vertexPositionIdx-1));
            }
            for (int i=3;i<f.size();i++){
                res.push_back((unsigned short)(f[0].vertexPositionIdx-1));
                res.push_back((unsigned short)(f[i-1].vertexPositionIdx-1));
                res.push_back((unsigned short)(f[i].vertexPositionIdx-1));
            }
        }
        return res;
    }

    std::shared_ptr<MeshData> ObjData::meshData() {
        auto meshData = make_shared<MeshData>();

        struct tuple_hash
        {
            std::size_t operator()(const tuple<int,int,int>& k) const
            {
                return (std::get<0>(k) << 16) | (std::get<1>(k) << 8) | std::get<2>(k);
            }
        };

        unordered_map<tuple<int, int, int>, int, tuple_hash> map;
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> ns;
        std::vector<unsigned short> indices;

        auto insert = [&](int i, ObjFace& f){
            tuple<int,int,int> t{f[i].vertexPositionIdx, f[i].textureIdx, f[i].normalIdx};
            auto res = map.find(t);
            if (res != map.end()){
                int index = res->second;
                indices.push_back(index);
            } else {
                int index = map.size();
                map[t] = index;
                indices.push_back((unsigned short) index);
                vertices.push_back((glm::vec3)vertexPositions[f[i].vertexPositionIdx-1]);
                if (textureCoords.size() > f[i].textureIdx-1){
                    uv.push_back((glm::vec2)textureCoords[f[i].textureIdx-1]);
                }
                if (normals.size() > f[i].normalIdx-1){
                    ns.push_back((glm::vec3)normals[f[i].normalIdx-1]);
                }
            }
        };

        for (auto & f : faces){
            assert (f.size()>=3);
            for (int i=0;i<3;i++){
                insert(i, f);
            }
            for (int i=3;i<f.size();i++){
                insert(0, f);
                insert(i-1, f);
                insert(i, f);
            }
        }

        meshData->setPosition(vertices);
        meshData->setSubmesh(0, indices, MeshType::Triangles);
        if (uv.size()>0){
            meshData->setTexCoord0(uv);
        }
        if (ns.size()==0){
            meshData->recomputeNormals();
        } else {
            if (ns[0] == glm::vec3{0}){
                logWarning("Invalid normal import.Was {0,0,0}");
                meshData->recomputeNormals();
            } else {
                meshData->setNormal(ns);
            }
        }

        return meshData;
    }
}