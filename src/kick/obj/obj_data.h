//
//  ObjData.h
//  KickObjLoader
//
//  Created by morten on 8/31/13.
//  Copyright (c) 2013 morten. All rights reserved.
//


#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include "kick/mesh/mesh_data.h"

namespace kick {
    struct ObjVertex {
        int vertexPositionIdx;
        int textureIdx;
        int normalIdx;
    };
    
    enum class ObjIlluminationMode {
        Mode0 = 0, // Color on and Ambient off
        Mode1 = 1, // Color on and Ambient on
        Mode2 = 2, // Highlight on
        Mode3 = 3, // Reflection on and Ray trace on
        Mode4 = 4, // Transparency: Glass on, Reflection: Ray trace on
        Mode5 = 5, // Reflection: Fresnel on and Ray trace on
        Mode6 = 6, // Transparency: Refraction on, Reflection: Fresnel off and Ray trace on
        Mode7 = 7, // Transparency: Refraction on, Reflection: Fresnel on and Ray trace on
        Mode8 = 8, // Reflection on and Ray trace off
        Mode9 = 9, // Transparency: Glass on, Reflection: Ray trace off
        Mode10 = 10 // Casts shadows onto invisible surfaces
    };
    
    enum class ObjTextureMapType {
        Diffuse, // map_Kd
        Ambient, // map_Ka
        Specular, // map_Ks
        SpecularCoeficient, // map_Ns
        Transparancy, // map_d or map_Tr
        Bump, // map_bump or bump
        Displacement, // disp
        StencilDecal // decal
    };
    
    struct ObjTextureMap {
        std::string filename;
        ObjTextureMapType type;
    };
    
    struct ObjMaterial {
        std::string name;
        glm::vec3 ambientColor; // Ka
        glm::vec3 diffuseColor; // Kd
        glm::vec3 specularColor; // Ks
        float specularCoefficient; // Ns
        float transparent; // d or Tr
        std::vector<ObjIlluminationMode> illuminationModes;
        std::vector<ObjTextureMap> textureMaps;
    };
    
    struct ObjMaterialChange {
        int faceIndex;
        std::string name;
    };
    
    typedef std::vector<ObjVertex> ObjFace;
    
    struct ObjGroup {
        int faceIndex;
        std::string name;
    };
    
    struct SmoothGroup {
        int faceIndex;
        int smoothGroupIdx; // 0 means none
    };
    
    struct ObjData {
        std::string initialComment;
        std::vector<glm::vec4> vertexPositions;
        std::vector<glm::vec3> textureCoords;
        std::vector<glm::vec3> normals;
        std::vector<ObjFace> faces;
        std::vector<ObjGroup> namedObjects;
        std::vector<ObjGroup> polygonGroups;
        std::vector<SmoothGroup> smoothGroups;
        std::vector<ObjMaterialChange> materialChanges;
        std::vector<ObjMaterial> materials;
        
        /// throws exception if invalid faces
        bool verifyFaces();
        
        ObjMaterial* getMaterial(std::string name);
        
        std::vector<float> getInterleavedVertexData(bool includeTextureCoords = true, bool includeNormals = true, int vertexPositionSize = 3, int texCoordinateSize = 2);

        std::vector<unsigned int> getIndices();

        MeshData* getMeshData();
    };

}
