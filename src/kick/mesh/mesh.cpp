//
//  mesh.cpp
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 07/12/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/mesh/mesh.h"
#include "kick/mesh/mesh_data.h"
#include "kick/core/log.h"
#include <vector>
#include <set>

using namespace std;

namespace kick {
    Mesh::Mesh()
    {
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
            if (openglUsingVao()) {
                glDeleteVertexArrays((GLsizei) array.size(), array.data());
            }
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

                updateArrayBufferStructure(shader);
                vertexArrayObject[shader] = vertexArrayObjectIdx;
                glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
            } else {
                GLuint vertexArrayObject = iter->second;
                glBindVertexArray(vertexArrayObject);
            }
        } else
#endif
        {
            glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
            updateArrayBufferStructure(shader);
        }
        // reassign buffers
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferId);
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
        if (submeshIndex >= submeshData.size()){
            logWarning("submesh not found");
            return;
        }
        auto data = submeshData[submeshIndex];
        GLenum mode = data.mode;
        GLsizei count = data.indexCount;
        GLenum type = data.type;
        const GLvoid * offset = data.dataOffset;

        if (count < 0) {
            glDrawArrays(mode, 0, -count);
        } else if (count > 0) {
            glDrawElements(mode, count, type, offset);
        }
    }
    
    std::string Mesh::name(){
        return mName;
    }
    
    void Mesh::setName(std::string n){
        mName = n;
    }

    void Mesh::setMeshData(MeshData *m){
        if (m != nullptr){
            interleavedFormat = m->interleavedFormat();
            submeshData = m->indicesFormat();
            updateMeshData(m);
        } else {
            interleavedFormat.clear();
        }
    }
    
    void Mesh::updateMeshData(MeshData *mesh_data){
        vector<float> data = mesh_data->interleavedData();

        if (data.size()){
            GLsizeiptr vertexDataSize =data.size()*sizeof(float);
            glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
            glBufferData(GL_ARRAY_BUFFER, vertexDataSize, data.data(), mesh_data->meshUsageVal());
        }

        vector<GLushort> indices = mesh_data->indicesConcat();
        if (indices.size()){
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferId);
            GLsizeiptr indicesSize = indices.size()*sizeof(GLushort);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices.data(), mesh_data->meshUsageVal());
        }
    }
};