//
//  shader.h
//  KickCPP
//
//  Created by morten on 8/25/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <glm/glm.hpp>
#include "kick/core/event.h"
#include "kick/core/kickgl.h"
#include "kick/core/project_asset.h"
#include "kick/mesh/mesh_data.h"

namespace kick {
    
    class Material;
    class EngineUniforms;
    class Transform;
    
    enum class ShaderType {
        VertexShader = GL_VERTEX_SHADER,
        FragmentShader = GL_FRAGMENT_SHADER,
        GeometryShader = GL_GEOMETRY_SHADER
    };
    
    enum class ShaderErrorType {
        VertexShader = GL_VERTEX_SHADER,
        FragmentShader = GL_FRAGMENT_SHADER,
        GeometryShader = GL_GEOMETRY_SHADER,
        Linker,
        IncompleteShader
    };
    
    enum class BlendType {
        Zero = GL_ZERO,
        One = GL_ONE,
        SrcColor = GL_SRC_COLOR,
        OneMinusSrcColor = GL_ONE_MINUS_SRC_COLOR,
        DstColor = GL_DST_COLOR,
        OneMinusDstColor = GL_ONE_MINUS_DST_COLOR,
        SrcAlpha = GL_SRC_ALPHA,
        OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
        DstAlpha = GL_DST_ALPHA,
        OneMinusDstAlpha = GL_ONE_MINUS_DST_ALPHA,
        ConstantColor = GL_CONSTANT_COLOR,
        OneMinusConstantColor = GL_ONE_MINUS_CONSTANT_COLOR,
        ConstantAlpha = GL_CONSTANT_ALPHA,
        OneMinusConstantAlpha = GL_ONE_MINUS_CONSTANT_ALPHA
    };
    
    enum class FaceCullingType {
        Front = GL_FRONT,
        Back = GL_BACK,
        FrontAndBack = GL_FRONT_AND_BACK,
        None = GL_NONE
    };
    
    enum class ZTestType {
        Never = GL_NEVER,
        Less = GL_LESS,
        Equal = GL_EQUAL,
        LEqual = GL_LEQUAL,
        Greater = GL_GREATER,
        NotEqual = GL_NOTEQUAL,
        GEqual = GL_GEQUAL,
        Always = GL_ALWAYS
    };
    
    bool toEnum(int value, ShaderType& enumValue);
    bool toEnum(int value, ShaderErrorType& enumValue);
    bool toEnum(int value, BlendType& enumValue);
    bool toEnum(int value, FaceCullingType& enumValue);
    bool toEnum(int value, ZTestType& enumValue);
    bool toEnum(int value, FaceCullingType& enumValue);
    
    class ShaderObj {
    public:
        ShaderObj() {}
        explicit ShaderObj(ShaderType type);
        ~ShaderObj();
        ShaderObj(ShaderObj&&);
        ShaderObj(const ShaderObj&) = delete;
        ShaderObj& operator=(ShaderObj&& other);
        void detach(GLuint shaderProgram);
        operator GLuint() { return shader; }
    private:
        GLuint shader = 0;
    };

    
    struct ShaderBuildException {
        std::string errorMessage;
        ShaderErrorType errorType;
        std::string codeSnippet;
        static void throwCurrentCompileException(GLuint shader, ShaderErrorType type, std::string source);
        static std::string extractLines(std::string errorMessage, std::string source, int extraLines = 2);
    };
    
    struct AttributeDescriptor  {
        GLuint index;
        GLint size;
        GLenum type;
        std::string name;
        VertexAttributeSemantic semantic;
    };
    
    struct UniformDescriptor  {
        GLuint index;
        GLint size;
        GLenum type;
        std::string name;
    };
    
    namespace UniformNames {
        const std::string modelMatrix{"_modelMatrix"};
        const std::string mv{"_mv"};
        const std::string norm{"_norm"};
        const std::string v{"_v"};
        const std::string worldCamPos{"_worldCamPos"};
        const std::string world2object{"_world2object"};
        const std::string mvProj{"_mvProj"};
        const std::string gameObjectUID{"_gameObjectUID"};
        const std::string shadowMapTexture{"_shadowMapTexture"};
        const std::string lightMat{"_lightMat"};
        
        const static std::string list[] = {
            modelMatrix,
            mv,
            norm,
            v,
            worldCamPos,
            world2object,
            mvProj,
            gameObjectUID,
            shadowMapTexture,
            lightMat
        };
    };
    
    class Project;
    
    class Shader : public ProjectAsset {
    public:
        /// If vertexShader and fragmentShader is specified then apply is called
        Shader(Project *p, std::string vertexShader = "", std::string fragmentShader = "", std::string geometryShader = "");
        Shader(Shader&& s);
        Shader(const Shader&) = delete;
        ~Shader();
        Shader& operator=(Shader&& other);
        void setShaderSource(ShaderType type, std::string str);
        std::string getShaderSource(ShaderType type) const;
        /// throws ShaderBuildException if unsuccessfull
        void apply();
        void bind();
        const std::vector<AttributeDescriptor >& getShaderAttributes() const;
        const std::vector<UniformDescriptor >& getShaderUniforms() const;
        const AttributeDescriptor* getShaderAttribute(VertexAttributeSemantic semantic) const;
        const UniformDescriptor* getShaderUniform(std::string name) const;
        Event<Shader*> shaderProgramChanged;
        void setBlend(bool b);
        bool getBlend();
        BlendType getBlendDFactorAlpha();
        BlendType getBlendDFactorRGB();
        BlendType getBlendSFactorAlpha();
        BlendType getBlendSFactorRGB();
        void setBlendDFactorAlpha(BlendType blendDFactorAlpha);
        void setBlendDFactorRGB(BlendType blendDFactorRGB);
        void setBlendSFactorAlpha(BlendType blendSFactorAlpha);
        void setBlendSFactorRGB(BlendType blendSFactorRGB);
        void setDepthMask(bool depthMask);
        bool getDepthMash();
        void setFaceCulling(FaceCullingType faceCulling);
        FaceCullingType getFaceCulling();
        void setPolygonOffsetEnabled(bool polygonOffsetEnabled);
        bool getPolygonOffsetEnabled();
        void setPolygonOffsetFactorAndUnit(glm::vec2 polygonOffsetFactorAndUnit);
        glm::vec2 getPolygonOffsetFactorAndUnit();
        void setZTest(ZTestType zTest);
        ZTestType getZTest();
        const std::vector<AttributeDescriptor>& getShaderAttributes() { return shaderAttributes; }
        void bind_uniforms(Material *material, EngineUniforms *engineUniforms, Transform* transform);
        GLuint getShaderProgram(){ return shaderProgram; }
        static std::string getPrecompiledSource(std::string source);
    private:
        /// throws ShaderBuildException if unsuccessfull
        void linkProgram();
        /// throws ShaderBuildException if unsuccessfull
        ShaderObj compileShader(std::string source, ShaderType type);
        GLuint shaderProgram = 0;
        std::map<ShaderType, std::string> shaderSources;
        std::string outputAttributeName = "fragColor";
        std::vector<AttributeDescriptor> shaderAttributes;
        std::vector<UniformDescriptor> shaderUniforms;
        bool blend {false};
        BlendType blendDFactorAlpha {BlendType::OneMinusSrcAlpha};
        BlendType blendDFactorRGB {BlendType::OneMinusSrcAlpha};
        BlendType blendSFactorAlpha {BlendType::SrcAlpha};
        BlendType blendSFactorRGB {BlendType::SrcAlpha};
        bool depthMask {true};
        FaceCullingType faceCulling {FaceCullingType::Back};
        bool polygonOffsetEnabled {false};
        glm::vec2 polygonOffsetFactorAndUnit {2.5, 10};
        ZTestType zTest {ZTestType::Less};
    };
    
    std::string to_string(AttributeDescriptor attDesc);
    
    std::string to_string(UniformDescriptor uniformDesc);

}
