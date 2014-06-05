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
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include "kick/core/event.h"
#include "kick/core/kickgl.h"
#include "kick/core/project_asset.h"
#include "kick/mesh/mesh_data.h"
#include "kick/material/shader_enums.h"
#include "kick/material/material.h"
namespace kick {
    
    class Material;
    class EngineUniforms;
    class Transform;

    class Project;
    class Shader;



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
        std::string getPrecompiledShaderSource(ShaderType type) const;
        /// throws ShaderBuildException if unsuccessfull
        void apply();
        void bind();
        const std::vector<AttributeDescriptor >& getShaderAttributes() const;
        const std::vector<UniformDescriptor >& getShaderUniforms() const;
        const AttributeDescriptor* getShaderAttribute(VertexAttributeSemantic semantic) const;
        const UniformDescriptor* getShaderUniform(std::string name) const;
        Event<ShaderEvent> shaderChanged;
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
        void setDepthWrite(bool depthMask);
        bool getDepthWrite();
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
        // Default uniform are assigned to materials where uniforms are not mapped
        template <class E>
        void setDefaultUniform(std::string name, E value);
        bool tryGetDefaultUniform(std::string name, MaterialData& value);
        static std::string getPrecompiledSource(std::string source);
    private:
        void updateShaderLocation(std::string name, MaterialData& value);
        void setDefaultUniformData(std::string name, MaterialData&& value);
        void updateDefaultShaderLocation();
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
        bool depthBufferWrite{true};
        FaceCullingType faceCulling {FaceCullingType::Back};
        bool polygonOffsetEnabled {false};
        glm::vec2 polygonOffsetFactorAndUnit {2.5, 10};
        ZTestType zTest {ZTestType::Less};
        std::map<std::string, MaterialData> defaultUniformData;
    };


    template <class E>
    void Shader::setDefaultUniform(std::string name, E value){
        MaterialData data{value};
        setDefaultUniformData(name, std::move(data));
    }
}
