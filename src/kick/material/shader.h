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
        Shader(std::string vertexShader = "", std::string fragmentShader = "", std::string geometryShader = "");
        Shader(Shader&& s);
        Shader(const Shader&) = delete;
        ~Shader();
        Shader& operator=(Shader&& other);
        void setShaderSource(ShaderType type, std::string str);
        std::string getShaderSource(ShaderType type) const;
        std::string getPrecompiledShaderSource(ShaderType type) const;
        /// throws ShaderBuildException if unsuccessfull
        bool apply();
        void bind();
        const std::vector<AttributeDescriptor >& getShaderAttributes() const;
        const std::vector<UniformDescriptor >& getShaderUniforms() const;
        const AttributeDescriptor* getShaderAttribute(VertexAttributeSemantic semantic) const;
        const UniformDescriptor* getShaderUniform(std::string name) const;
        Event<ShaderEvent> shaderChanged;
        void setBlend(bool b);
        bool blend();
        BlendType blendDFactorAlpha();
        BlendType blendDFactorRGB();
        BlendType blendSFactorAlpha();
        BlendType blendSFactorRGB();
        void setBlendDFactorAlpha(BlendType blendDFactorAlpha);
        void setBlendDFactorRGB(BlendType blendDFactorRGB);
        void setBlendSFactorAlpha(BlendType blendSFactorAlpha);
        void setBlendSFactorRGB(BlendType blendSFactorRGB);
        void setDepthWrite(bool depthMask);
        bool depthWrite();
        void setFaceCulling(FaceCullingType faceCulling);
        FaceCullingType faceCulling();
        void setPolygonOffsetEnabled(bool polygonOffsetEnabled);
        bool polygonOffsetEnabled();
        void setPolygonOffsetFactorAndUnit(glm::vec2 polygonOffsetFactorAndUnit);
        glm::vec2 polygonOffsetFactorAndUnit();
        void setZTest(ZTestType zTest);
        ZTestType zTest();
        const std::vector<AttributeDescriptor>&shaderAttributes() { return mShaderAttributes; }
        void bind_uniforms(Material *material, EngineUniforms *engineUniforms, Transform* transform);
        GLuint shaderProgram(){ return mShaderProgram; }

        void setDefaultUniform(std::string name, int value);
        void setDefaultUniform(std::string name, float value);
        void setDefaultUniform(std::string name, glm::vec4 value);
        void setDefaultUniform(std::string name, glm::mat3 value);
        void setDefaultUniform(std::string name, glm::mat4 value);
        void setDefaultUniform(std::string name, std::shared_ptr<Texture2D> value);
        void setDefaultUniform(std::string name, std::shared_ptr<TextureCube> value);

        bool tryGetDefaultUniform(std::string name, MaterialData& value);
        static std::string getPrecompiledSource(std::string source, ShaderType type);

        int getRenderOrder() const;
        void setRenderOrder(int renderOrder);
    private:
        // Default uniform are assigned to materials where uniforms are not mapped
        template <class E>
        void setDefaultUniformInternal(std::string name, E value);
        static void translateToGLSLES(std::string& s, ShaderType type);
        void updateShaderLocation(std::string name, MaterialData& value);
        void setDefaultUniformData(std::string name, MaterialData&& value);
        void updateDefaultShaderLocation();
        /// throws ShaderBuildException if unsuccessfull
        bool linkProgram();
        /// throws ShaderBuildException if unsuccessfull
        ShaderObj compileShader(std::string source, ShaderType type);
        GLuint mShaderProgram = 0;
        std::map<ShaderType, std::string> shaderSources;
        std::string outputAttributeName = "fragColor";
        std::vector<AttributeDescriptor> mShaderAttributes;
        std::vector<UniformDescriptor> shaderUniforms;
        bool mBlend{false};
        BlendType mBlendDFactorAlpha{BlendType::OneMinusSrcAlpha};
        BlendType mBlendDFactorRGB{BlendType::OneMinusSrcAlpha};
        BlendType mBlendSFactorAlpha{BlendType::SrcAlpha};
        BlendType mBlendSFactorRGB{BlendType::SrcAlpha};
        bool mDepthBufferWrite{true};
        FaceCullingType mFaceCulling{FaceCullingType::Back};
        bool mPolygonOffsetEnabled{false};
        glm::vec2 mPolygonOffsetFactorAndUnit{2.5, 10};
        ZTestType mZTest{ZTestType::Less};
        std::map<std::string, MaterialData> defaultUniformData;
        int mDenderOrder = 1000;
        std::map<std::string, std::shared_ptr<Texture2D>> texture2DRef;
        std::map<std::string, std::shared_ptr<TextureCube>> textureCubeRef;
    };

    template <class E>
    void Shader::setDefaultUniformInternal(std::string name, E value){
        MaterialData data{value};
        setDefaultUniformData(name, std::move(data));
    }
}
