//
//  ObjData.cpp
//  KickObjLoader
//
//  Created by morten on 8/31/13.
//  Copyright (c) 2013 morten. All rights reserved.
//

#include "obj_data.h"
#include <stdexcept>
#include <string>
#include "kick/core/log.h"

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
    
    ObjMaterial* ObjData::getMaterial(std::string name){
        for (auto & mat : materials){
            if (mat.name == name){
                return &mat;
            }
        }
        return nullptr;
    }

    std::vector<unsigned int> ObjData::getIndices() {
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

    std::vector<glm::vec3> ObjData::getVertexPositionVec3() {
        std::vector<glm::vec3> res;
        for (auto p : vertexPositions){
            res.push_back((glm::vec3)p);
        }
        return res;
    }

    std::vector<glm::vec2> ObjData::getTextureCoordsVec2() {
        std::vector<glm::vec2> res;
        for (auto t : textureCoords){
            res.push_back((glm::vec2)t);
        }
        return res;
    }

    std::vector<unsigned short> ObjData::getIndicesUS() {
        std::vector<unsigned short> res;
        for (auto & f : faces){
            assert (f.size()>=3);
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
}