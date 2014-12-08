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
        glGenBuffers(1, &mVertexBufferId);
        glGenBuffers(1, &mElementBufferId);
    }
    
    Mesh::~Mesh(){
        if (mVertexArrayObject.size()>0){
            vector<GLuint> array(mVertexArrayObject.size());
            for (auto keyValue : mVertexArrayObject){
                array.push_back(keyValue.second);
            }
#ifndef GL_ES_VERSION_2_0
            if (openglUsingVao()) {
                glDeleteVertexArrays((GLsizei) array.size(), array.data());
            }
#endif
        }
        glDeleteBuffers(1, &mVertexBufferId);
        glDeleteBuffers(1, &mElementBufferId);
    }
    
    void Mesh::bind(Shader * shader){
        shader->bind();

#ifndef GL_ES_VERSION_2_0
        if (openglUsingVao()){
            auto iter = mVertexArrayObject.find(shader);
            if (iter == mVertexArrayObject.end()){
                // create and bind object
                GLuint vertexArrayObjectIdx;

                glGenVertexArrays(1, &vertexArrayObjectIdx);
                glBindVertexArray(vertexArrayObjectIdx);
                updateArrayBufferStructure(shader);
                mVertexArrayObject[shader] = vertexArrayObjectIdx;
            } else {
                GLuint vertexArrayObject = iter->second;
                glBindVertexArray(vertexArrayObject);
            }
        } else
#endif
        {
            updateArrayBufferStructure(shader);
        }
        // reassign buffers
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mElementBufferId);
    }
    
    void Mesh::updateArrayBufferStructure(Shader *shader){
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferId);
        static int maxAtrrSizes = -1;
        if (maxAtrrSizes == -1){
            glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAtrrSizes);
            if (maxAtrrSizes>32){
                maxAtrrSizes = 32;
            }
        }
        bitset<32> enabledArrays;

        for (InterleavedRecord format_record : mInterleavedFormat) {
            const AttributeDescriptor * desc = shader->getShaderAttribute(format_record.semantic);
            if (desc){
                glEnableVertexAttribArray(desc->index);
                glVertexAttribPointer(desc->index, format_record.size,
                                       format_record.type, (GLboolean) format_record.normalized, format_record.stride, format_record.offset);
                enabledArrays[desc->index] = true;
            }
        }
        for (int i=0;i<maxAtrrSizes;i++){
            if (enabledArrays[i] == false){
                glDisableVertexAttribArray(i);
            }
        }
    }
    
    void Mesh::render(unsigned int submeshIndex){
        if (submeshIndex >= mSubmeshData.size()){
            logWarning("submesh not found");
            return;
        }
        auto data = mSubmeshData[submeshIndex];
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

    void Mesh::setMeshData(shared_ptr<MeshData> m){
        if (m != nullptr){
            mInterleavedFormat = m->interleavedFormat();
            mSubmeshData = m->indicesFormat();
            updateMeshData(m.get());
        } else {
            mInterleavedFormat.clear();
        }
        mMeshData = m;
    }
    
    void Mesh::updateMeshData(MeshData *mesh_data){
        vector<float> data = mesh_data->interleavedData();

        if (data.size()){
            GLsizeiptr vertexDataSize =data.size()*sizeof(float);
            glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferId);
            glBufferData(GL_ARRAY_BUFFER, vertexDataSize, data.data(), mesh_data->meshUsageVal());
        }

        vector<GLushort> indices = mesh_data->indicesConcat();
        if (indices.size()){
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mElementBufferId);
            GLsizeiptr indicesSize = indices.size()*sizeof(GLushort);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices.data(), mesh_data->meshUsageVal());
        }
    }

    std::shared_ptr<MeshData> Mesh::meshData() {
        return mMeshData;
    }
};