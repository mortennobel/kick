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

using namespace std;

namespace kick {
    bool ObjData::verifyFaces() {
        for (auto & face : faces) {
            if (face.size()<3){
                throw length_error("Faces must be at least 3 vertices");
            }
            
            for (auto & vertex : face) {
                if (vertex.vertexPositionIdx > vertexPositions.size()){
                    throw length_error(string{"Invalid vertex position index. Index was "} +
                                       std::to_string(vertex.vertexPositionIdx) +
                                       " number of vertex positions is "+
                                       std::to_string(vertexPositions.size()));
                }
                
                if (vertex.textureIdx > textureCoords.size()){
                    throw length_error(string{"Invalid texture index. Index was "} +
                                       std::to_string(vertex.textureIdx) +
                                       " number of texture indices is "+
                                       std::to_string(textureCoords.size()));
                }
                
                if (vertex.normalIdx > normals.size()){
                    throw length_error(string{"Invalid normal index. Index was "} +
                                       std::to_string(vertex.normalIdx) +
                                       " number of normals is "+
                                       std::to_string(normals.size()));
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
            for (auto &v : f){
                res.push_back(v.vertexPositionIdx-1);
            }
        }
        return res;
    }
}