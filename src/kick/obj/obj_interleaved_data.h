//
//  ObjInterleavedData.h
//  KickObjLoader
//
//  Created by morten on 9/2/13.
//  Copyright (c) 2013 morten. All rights reserved.
//

#pragma once
#include "obj_data.h"
#include <iostream>
#include <vector>
#include <string>

namespace kick {
    struct ObjInterleavedIndex {
        std::string materialName;
        std::vector<int> vertexIndices;
    };
    
    struct ObjInterleavedData {
        std::vector<float> interleavedData;
        std::vector<ObjInterleavedIndex> indices;
        
        ObjInterleavedData(){}
        
        // Creates interleaved triangles
        ObjInterleavedData(ObjData & objData, bool includeTextureCoordinates = true, bool includeNormals = true, int vertexPositionSize = 3, int texCoordinateSize = 2);
        
        bool includeTextureCoordinates = true;
        bool includeNormals = true;
        int vertexPositionSize = 3;
        int texCoordinateSize = 2;
        int vertexLength;
        const int vertexPosOffset = 0;
        int textureOffset = 0;
        int normalsOffset = 0;
        
        int getVertexCount();
        glm::vec4 vertex(int index);
        void setVertex(int index, glm::vec4 value);
        glm::vec3 textureCoordinate(int index);
        void setTextureCoordinate(int index, glm::vec3 value);
        glm::vec3 normal(int index);
        void setNormal(int index, glm::vec3 value);
    };
}