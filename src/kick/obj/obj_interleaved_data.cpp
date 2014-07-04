//
//  ObjInterleavedData.cpp
//  KickObjLoader
//
//  Created by morten on 9/2/13.
//  Copyright (c) 2013 morten. All rights reserved.
//

#include "obj_interleaved_data.h"
#include <unordered_map>
#include <glm/glm.hpp>
#include <cassert>
#include <algorithm>

using namespace std;
using namespace glm;

namespace kick {
    
    struct ObjVertexHash {
        std::size_t operator()(const ObjVertex& k) const {
            return (( k.vertexPositionIdx << 16 ) ^ ( k.textureIdx << 8 )) ^ k.normalIdx;
        }
    };
    
    struct ObjVertexEqual {
        bool operator()(const ObjVertex& lhs, const ObjVertex& rhs) const {
            return lhs.vertexPositionIdx == rhs.vertexPositionIdx &&
            lhs.textureIdx == rhs.textureIdx &&
            lhs.normalIdx == rhs.normalIdx;
        }
    };
    
    using ObjVertexHashTable = unordered_map<ObjVertex,int,ObjVertexHash, ObjVertexEqual>;
    
    int getCreateIndex(ObjVertex &vertexIndex, int nextIndex, ObjVertexHashTable& usedVertices){
        auto pos = usedVertices.find(vertexIndex);
        if (pos != usedVertices.end()){
            return pos->second;
        }
        usedVertices.emplace(vertexIndex, nextIndex);
        return nextIndex;
    }
    
    ObjInterleavedData::ObjInterleavedData(ObjData & objData, bool includeTextureCoordinates, bool includeNormals, int vertexPositionSize, int texCoordinateSize)
    :includeTextureCoordinates(includeTextureCoordinates), includeNormals(includeNormals), vertexPositionSize(vertexPositionSize), texCoordinateSize(texCoordinateSize) {
        
        vertexLength = vertexPositionSize;
        if (includeTextureCoordinates){
            textureOffset = vertexLength;
            vertexLength += texCoordinateSize;
        }
        if (includeNormals){
            normalsOffset = vertexLength;
            vertexLength += 3;
        }
        
        indices.push_back({"___Default material___", {}});
        
        ObjInterleavedIndex * currentIndex = &indices.back();
        ObjVertexHashTable usedVertices{42,ObjVertexHash{}, ObjVertexEqual{}};
        
        int vertexCount = 0;
        int faceCount = 0;
        auto materialChanges = objData.materialChanges.begin();
        for (auto & face : objData.faces){
            faceCount++;
            if (materialChanges != objData.materialChanges.end()){
                if (materialChanges->faceIndex == faceCount){
                    bool foundMaterial = false;
                    for (auto & idx : indices) {
                        if (idx.materialName == materialChanges->name){
                            foundMaterial = true;
                            currentIndex = &idx;
                            break;
                        }
                    }
                    if (!foundMaterial){
                        indices.push_back({materialChanges->name, {}});
                        currentIndex = &indices.back();
                    }
                    materialChanges++;
                }
            }
            for (int i=2;i < face.size();i++){
                int triangle[] = {0, i-1, i};
                for (int triangleIndex : triangle){
                    auto & vertexIndexObject = face[triangleIndex];
                    int vertexIndex = getCreateIndex(vertexIndexObject, vertexCount, usedVertices);
                    bool existInInterleavedData = vertexIndex != vertexCount;
                    if (!existInInterleavedData){
                        // read data
                        vec4 vertexPosition = objData.vertexPositions[vertexIndexObject.vertexPositionIdx - 1];
                        vec3 textureCoord{0,0,0};
                        if (vertexIndexObject.textureIdx > 0 && includeTextureCoordinates){
                            textureCoord = objData.textureCoords[vertexIndexObject.textureIdx - 1];
                        }
                        vec3 normal;
                        if (vertexIndexObject.normalIdx > 0 && includeNormals){
                            normal = objData.normals[vertexIndexObject.normalIdx-1];
                        }
                        // write interleaved data
                        for (int j=0;j<vertexPositionSize;j++){
                            interleavedData.push_back(vertexPosition[j]);
                        }
                        if (includeTextureCoordinates){
                            for (int j=0;j<texCoordinateSize;j++){
                                interleavedData.push_back(textureCoord[j]);
                            }
                        }
                        if (includeNormals){
                            for (int j=0;j<3;j++){
                                interleavedData.push_back(normal[j]);
                            }
                        }
                        vertexCount++;
                    }
                    currentIndex->vertexIndices.push_back(vertexIndex);
                }
            }
        }
        
        // remove unused materials
        indices.erase(std::remove_if(indices.begin(), indices.end(), [](const ObjInterleavedIndex &a){ return a.vertexIndices.size()==0;}),
                   indices.end());
    }
    
    int ObjInterleavedData::getVertexCount(){
        return (int)(interleavedData.size() / vertexLength);
    }
    
    glm::vec4 ObjInterleavedData::vertex(int index){
        int i = vertexLength*index;
        return glm::vec4{interleavedData[i], interleavedData[i+1], interleavedData[i+2], vertexPositionSize==4?interleavedData[i+3]:1};
    }
    
    void ObjInterleavedData::setVertex(int index, glm::vec4 value){
        int i = vertexLength*index;
        for (int j=0;j<vertexPositionSize;j++){
            interleavedData[i+j] = value[j];
        }
    }
    glm::vec3 ObjInterleavedData::textureCoordinate(int index){
        int i = vertexLength*index + textureOffset;
        return glm::vec3{interleavedData[i], interleavedData[i+1], texCoordinateSize==3?interleavedData[i+2]:0};
    }
    void ObjInterleavedData::setTextureCoordinate(int index, glm::vec3 value){
        int i = vertexLength*index + textureOffset;
        for (int j=0;j<texCoordinateSize;j++){
            interleavedData[i+j] = value[j];
        }
    }
    glm::vec3 ObjInterleavedData::normal(int index){
        int i = vertexLength*index + normalsOffset;
        return glm::vec3{interleavedData[i], interleavedData[i+1], interleavedData[i+2]};
    }
    
    void ObjInterleavedData::setNormal(int index, glm::vec3 value){
        int i = vertexLength*index + normalsOffset;
        for (int j=0;j<3;j++){
            interleavedData[i+j] = value[j];
        }
    }
}
