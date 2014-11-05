//
//  MeshData.h
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 10/20/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once

#include "kick/core/project_asset.h"
#include "kick/math/bounds3.h"
#include "kick/core/kickgl.h"
#include <vector>
#include <unordered_map>
#include <string>

#include <glm/glm.hpp>
#include <exception>

namespace kick {
    enum class MeshType {
        Triangles = GL_TRIANGLES,
        TriangleFan = GL_TRIANGLE_FAN,
        TriangleStrip = GL_TRIANGLE_STRIP,
        Points = GL_POINTS,
        Lines = GL_LINES,
        LineLoop = GL_LINE_LOOP,
        LineStrip = GL_LINE_STRIP,
    };
    
    enum class MeshUsage {

        StreamDraw = GL_STREAM_DRAW,
#ifndef GL_ES_VERSION_2_0
        StreamRead = GL_STREAM_READ,
        StreamCopy = GL_STREAM_COPY,
#endif
        StaticDraw = GL_STATIC_DRAW,
#ifndef GL_ES_VERSION_2_0
        StaticRead = GL_STATIC_READ,
        StaticCopy = GL_STATIC_COPY,
#endif
        DynamicDraw = GL_DYNAMIC_DRAW,
#ifndef GL_ES_VERSION_2_0
        DynamicRead = GL_DYNAMIC_READ,
        DynamicCopy = GL_DYNAMIC_COPY
#endif
    };
    
    enum class VertexAttributeSemantic {
        Position,
        Normal,
        Uv1,
        Uv2,
        Tangent,
        Color,
        Unknown
    };
    
    VertexAttributeSemantic to_semantic(std::string name);
    std::string to_string(VertexAttributeSemantic semantic);
    
    struct InterleavedRecord {
        const VertexAttributeSemantic semantic;
        const GLvoid * offset; // offset pointer
        const int size; // number of (float/int) elements
        const bool normalized; // should be normalized or not
        const GLenum type; // {GL_FLOAT or GL_INT}
        GLsizei stride;
    };
    
    struct SubMeshData {
        const GLsizei indexCount;
        const GLvoid *dataOffset;
        const GLenum mode;
        const GLenum type;
    };
    
    /**
     * MeshData represent data on the CPU side.
     */
    class MeshData : public ProjectAsset {
    public:
        MeshData();
        void setBounds(const Bounds3 &mBounds);
        const Bounds3 &bounds();

        GLenum meshUsageVal();

        void setPosition(const std::vector<glm::vec3> &p);
        const std::vector<glm::vec3>&position();
        void setNormal(const std::vector<glm::vec3> &n);
        const std::vector<glm::vec3>&normal();
        void setTexCoord0(const std::vector<glm::vec2> &u1);
        const std::vector<glm::vec2>&texCoord0();
        void setTexCoord1(const std::vector<glm::vec2> &u2);
        const std::vector<glm::vec2>&texCoord1();
        void setTangent(const std::vector<glm::vec3> &t);
        const std::vector<glm::vec3>&tangent();
        void setColor(const std::vector<glm::vec4> &c);
        const std::vector<glm::vec4>&color();
        
        void setSubmesh(unsigned int index, const std::vector<GLushort> &indices, MeshType meshType);
        const std::vector<GLushort>&submeshIndices(unsigned int index) const;

        const MeshType submeshType(unsigned int index) const;
        
        unsigned int submeshesCount();
        GLsizei submeshSize(unsigned int index);
        
        std::vector<float> interleavedData();
        std::vector<InterleavedRecord> interleavedFormat();
        std::vector<GLushort> indicesConcat();
        std::vector<SubMeshData> indicesFormat();
        
        // recomputing normals (using angle weighted normals)
        void recomputeNormals();

        void recomputeBounds();
        MeshUsage meshUsage() const;
        void setMeshUsage(MeshUsage meshUsage);
    private:
        Bounds3 mBounds;
        MeshUsage mMeshUsage = MeshUsage::StaticDraw;
        std::vector<glm::vec3> mPosition;
        std::vector<glm::vec3> mNormal;
        std::vector<glm::vec2> mTexCoord0;
        std::vector<glm::vec2> mTexCoord1;
        std::vector<glm::vec3> mTangent;
        std::vector<glm::vec4> mColor;
        
        struct SubMeshInternal {
            std::vector<GLushort> indices;
            MeshType meshType;
        };
        
        std::vector<SubMeshInternal> subMeshes;
        size_t computeInterleavedDataSize();
    };
}