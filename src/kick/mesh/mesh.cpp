//
//  mesh.cpp
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 07/12/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "mesh.h"
#include "kick/mesh/mesh_data.h"
#include <vector>
#include <set>

using namespace std;

namespace kick {
    Mesh::Mesh(Project* project)
    :ProjectAsset{project}{
        glGenBuffers(1, &vertexBufferId);
        glGenBuffers(1, &elementBufferId);
    }
    
    Mesh::~Mesh(){
        if (vertexArrayObject.size()>0){
            vector<GLuint> array(vertexArrayObject.size());
            for (auto keyValue : vertexArrayObject){
                array.push_back(keyValue.second);
            }
#ifndef GL_ES_VERSION_2_0
            glDeleteVertexArrays((GLsizei)array.size(), array.data());
#endif
        }
        glDeleteBuffers(1, &vertexBufferId);
        glDeleteBuffers(1, &elementBufferId);
    }
    
    void Mesh::bind(Shader * shader){
        shader->bind();
#ifndef GL_ES_VERSION_2_0
        if (openglUsingVao()){

            auto iter = vertexArrayObject.find(shader);
            if (iter == vertexArrayObject.end()){
                // create and bind object
                GLuint vertexArrayObjectIdx;

                glGenVertexArrays(1, &vertexArrayObjectIdx);
                glBindVertexArray(vertexArrayObjectIdx);

                // reassign buffers
                glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferId);

                updateArrayBufferStructure(shader);
                vertexArrayObject[shader] = vertexArrayObjectIdx;
            } else {
                GLuint vertexArrayObject = iter->second;
                glBindVertexArray(vertexArrayObject);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferId);
            }

        } else
#endif
        {
            updateArrayBufferStructure(shader);
        }
        
    }
    
    void Mesh::updateArrayBufferStructure(Shader *shader){
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
        for (InterleavedRecord format_record : interleavedFormat) {
            const AttributeDescriptor * desc = shader->getShaderAttribute(format_record.semantic);
            if (desc){
                glEnableVertexAttribArray(desc->index);
                glVertexAttribPointer(desc->index, format_record.size,
                                       format_record.type, (GLboolean) format_record.normalized, format_record.stride, format_record.offset);
                
            }
        }
    }
    
    void Mesh::render(unsigned int submeshIndex){
        auto data = submeshData[submeshIndex];
        GLenum mode = data.mode;
        GLsizei count = data.indexCount;
        GLenum type = data.type;
        const GLvoid * offset = data.dataOffset;
        glDrawElements(mode, count, type, offset);
    }
    
    std::string Mesh::getName(){
        return name;
    }
    
    void Mesh::setName(std::string n){
        name = n;
    }
    
    MeshData *Mesh::getMeshData() {
        return mesh_data;
    }
    
    void Mesh::setMeshData(MeshData *m){
        mesh_data = m;
        if (m != nullptr){
            interleavedFormat = m->getInterleavedFormat();
            submeshData = m->getIndicesFormat();
            updateMeshData();
        } else {
            interleavedFormat.clear();
        }
    }
    
    void Mesh::updateMeshData(){
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
        vector<float> data = mesh_data->getInterleavedData();
        GLsizeiptr vertexDataSize =data.size()*sizeof(float);
        glBufferData(GL_ARRAY_BUFFER, vertexDataSize, data.data(), mesh_data->getMeshUsageVal());
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferId);
        vector<GLushort> indices = mesh_data->getIndices();
        GLsizeiptr indicesSize = indices.size()*sizeof(GLushort);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices.data(), mesh_data->getMeshUsageVal());
    }
};