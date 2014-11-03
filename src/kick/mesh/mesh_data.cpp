//
//  MeshData.cpp
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 10/20/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/mesh/mesh_data.h"
#include <algorithm>
#include <iostream>
#include <locale> 
#include "kick/core/log.h"
using namespace std;
using namespace glm;


namespace kick {

    namespace { // helper functions
        // helper function that returns lowercase of string
        string to_lower(string s){
            std::locale loc;
            for (int i=0;i<s.size();i++){
                s[i] = std::tolower(s[i],loc);
            }
            return s;
        }

        /**
        * Add data to interleaved data and return the new offset (in float elements)
        */
        template<typename T>
        size_t add_interleaved_record(const std::vector<T>& data, vector<InterleavedRecord>& interleaved, size_t offset, VertexAttributeSemantic semantic){
            if (data.size() == 0){
                return offset;
            }
            int vectorLength = (int)data[0].length();

            InterleavedRecord record{
                    semantic,
                    BUFFER_OFFSET(offset),
                    vectorLength,
                    false,
                    GL_FLOAT};
            interleaved.push_back(record);
            return offset + sizeof(T);
        }
    }

    MeshData::MeshData()
    {
        setSubmesh(0, {}, MeshType::Triangles);
    }
    
    void MeshData::setAabb(const AABB &a){
        mAabb = a;
    }
 
    void MeshData::setPosition(const std::vector<glm::vec3> &p) {
        mAabb.clear();
        mPosition = p;
        for (auto v : p) {
            mAabb.addPoint(v);
        }
    }
    
    const std::vector<glm::vec3>& MeshData::position() {
        return mPosition;
    }
    
    void MeshData::setNormal(const std::vector<glm::vec3> &n) {
        mNormal = n;
    }
    
    const std::vector<glm::vec3>& MeshData::normal() {
        return mNormal;
    }
    
    void MeshData::setTexCoord0(const std::vector<glm::vec2> &u1) {
        mTexCoord0 = u1;
    }
    
    const std::vector<glm::vec2>& MeshData::texCoord0() {
        return mTexCoord0;
    }
    
    void MeshData::setTexCoord1(const std::vector<glm::vec2> &u2) {
        mTexCoord1 = u2;
    }
    
    const std::vector<glm::vec2>& MeshData::texCoord1() {
        return mTexCoord1;
    }
    
    void MeshData::setTangent(const std::vector<glm::vec3> &t) {
        mTangent = t;
    }
    
    const std::vector<glm::vec3>& MeshData::tangent() {
        return mTangent;
    }
    
    void MeshData::setColor(const std::vector<glm::vec4> &c) {
        mColor = c;
    }
    
    const std::vector<glm::vec4>& MeshData::color(){
        return mColor;
    }
    
    GLsizei MeshData::submeshSize(unsigned int index){
        return static_cast<GLsizei>(subMeshes[index].indices.size());
    }
    
    unsigned int MeshData::submeshesCount(){
        return static_cast<unsigned int>(subMeshes.size());
    }

    size_t MeshData::computeInterleavedDataSize(){
        size_t numberOfElements = static_cast<int>(mPosition.size());
        size_t vertexSize = 3;
        if (mNormal.size() > 0) {
            vertexSize += 3;
        }
        if (mTexCoord0.size() > 0) {
            vertexSize += 2;
        }
        if (mTexCoord1.size() > 0) {
            vertexSize += 2;
        }
        if (mTangent.size() > 0) {
            vertexSize += 4;
        }
        if (mColor.size() > 0) {
            vertexSize += 4;
        }
        return vertexSize * numberOfElements;
    }
    
    /**
     * Add data to interleaved data and return the new offset (in float elements)
     */
    template<typename T>
    size_t add_data(const std::vector<T>& data, vector<float>& interleaved, size_t offset, int stride){
        if (data.size() == 0){
            return offset;
        }
        size_t vectorLength = data[0].length();
        size_t index = offset;
        for (int i=0;i<data.size();i++){
            for (int j=0;j<vectorLength;j++){
                interleaved[index+j] = data[i][j];
            }
            index += stride;
        }
        return offset + vectorLength;
    }
    
    vector<float> MeshData::interleavedData(){
        const size_t floatElements = computeInterleavedDataSize();
        vector<float> res(floatElements);
        if (floatElements == 0){
            return res;
        }
        const int stride = floatElements / mPosition.size();
        size_t offset = add_data(mPosition, res, 0, stride);
        offset = add_data(mNormal, res, offset, stride);
        offset = add_data(mTexCoord0, res, offset, stride);
        offset = add_data(mTexCoord1, res, offset, stride);
        offset = add_data(mTangent, res, offset, stride);
        offset = add_data(mColor, res, offset, stride);
        return res;
    }

    vector<InterleavedRecord> MeshData::interleavedFormat() {
        vector<InterleavedRecord> res;
        size_t offset = add_interleaved_record(mPosition, res, 0, VertexAttributeSemantic::Position);
        offset = add_interleaved_record(mNormal, res, offset, VertexAttributeSemantic::Normal);
        offset = add_interleaved_record(mTexCoord0, res, offset, VertexAttributeSemantic::Uv1);
        offset = add_interleaved_record(mTexCoord1, res, offset, VertexAttributeSemantic::Uv2);
        offset = add_interleaved_record(mTangent, res, offset, VertexAttributeSemantic::Tangent);
        offset = add_interleaved_record(mColor, res, offset, VertexAttributeSemantic::Color);
        
        for (auto & record : res){
            record.stride = static_cast<GLsizei>(offset);
        }
        
        return res;
    }
    
    vector<GLushort> MeshData::indicesConcat(){
        vector<GLushort> res;
        for (auto & v : subMeshes){
            res.insert(res.end(), v.indices.begin(), v.indices.end());
        }
        return res;
    }
    
    vector<SubMeshData> MeshData::indicesFormat(){
        vector<SubMeshData> res;
        size_t offset = 0;
        for (auto & v : subMeshes){
            SubMeshData record{
                static_cast<GLsizei>(v.indices.size()),
                BUFFER_OFFSET(offset),
                static_cast<GLenum>(v.meshType),
                GL_UNSIGNED_SHORT
            };
            
            res.push_back(record);
            offset += sizeof(GLuint) * v.indices.size();
        }
        return res;
    }
    
    void MeshData::setSubmesh(unsigned int index, const std::vector<GLushort> &indices, MeshType meshType){
        while (subMeshes.size() <= index){
            subMeshes.push_back({{}, MeshType::Triangles});
        }
        subMeshes[index].indices = indices;
        subMeshes[index].meshType = meshType;
    }
    
    const vector<GLushort>& MeshData::submeshIndices(unsigned int index) const {
        return subMeshes.at(index).indices;
    }
    
    const MeshType MeshData::submeshType(unsigned int index) const{
        return subMeshes.at(index).meshType;
    }
    
    void MeshData::recomputeNormals(){
        // empty
        mNormal.resize(mPosition.size());
        for (int i=0;i< mNormal.size();i++){
            mNormal[i] = vec3{0};
        }
        
        for (auto & v : subMeshes){
            auto & submesh = v.indices;
            if (v.meshType != MeshType::Triangles){
//                throw std::invalid_argument();
                logError("Recalcualte normals only valid for triangles");
                return;
            }
            size_t triangleCount = submesh.size()/3;
            for (int a = 0; a < triangleCount; a++) {
                GLuint i1 = submesh[a * 3];
                GLuint i2 = submesh[a * 3 + 1];
                GLuint i3 = submesh[a * 3 + 2];
                
                vec3 v1 = mPosition[i1];
                vec3 v2 = mPosition[i2];
                vec3 v3 = mPosition[i3];
                
                vec3 v1v2 = normalize(v2 - v1);
                vec3 v1v3 = normalize(v3 - v1);
                
                vec3 faceNormal = normalize(cross(v1v2, v1v3));
                float weight1 = acos(std::max(-1.0f, std::min(1.0f, dot(v1v2, v1v3))));
                
                vec3 v2v3 = normalize(v3 - v2);
                float weight2 = (float) (M_PI - acos(std::max(-1.0f, std::min(1.0f, dot(v1v2, v2v3)))));
                float weight3 = (float) (M_PI - weight1 - weight2);
                mNormal[i1] += weight1 * faceNormal;
                mNormal[i2] += weight2 * faceNormal;
                mNormal[i3] += weight3 * faceNormal;
            }
        }
        for (int i=0;i< mNormal.size();i++){
            if (mNormal[i] != vec3{0}){
                mNormal[i] = normalize(mNormal[i]);
            }
        }
    }

    const AABB &MeshData::aabb() { return mAabb; }

    GLenum MeshData::meshUsageVal() { return static_cast<GLenum>(mMeshUsage); }


#define RETURN_IF_EQUAL(X) if (to_lower(name) == to_lower(#X)) return VertexAttributeSemantic::X;
    VertexAttributeSemantic to_semantic(std::string name){
        RETURN_IF_EQUAL(Position);
        RETURN_IF_EQUAL(Normal);
        RETURN_IF_EQUAL(Uv1);
        RETURN_IF_EQUAL(Uv2);
        RETURN_IF_EQUAL(Tangent);
        RETURN_IF_EQUAL(Color);
        return VertexAttributeSemantic::Unknown;
    }

    std::string to_string(VertexAttributeSemantic semantic){
        switch (semantic) {
            case VertexAttributeSemantic::Position:
                return "Position";
            case VertexAttributeSemantic::Normal:
                return "Normal";
            case VertexAttributeSemantic::Uv1:
                return "Uv1";
            case VertexAttributeSemantic::Uv2:
                return "Uv2";
            case VertexAttributeSemantic::Tangent:
                return "Tangent";
            case VertexAttributeSemantic::Color:
                return "Color";
            case VertexAttributeSemantic::Unknown:
                return "Unknown";
            default:
                //throw invalid_argument("Unknown semantic");
                logWarning("Unknown semantic");
                return "Unknown";
        }
    }

    MeshUsage MeshData::meshUsage() const {
        return mMeshUsage;
    }

    void MeshData::setMeshUsage(MeshUsage meshUsage) {
        MeshData::mMeshUsage = meshUsage;
    }
}

