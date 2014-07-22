//
//  material.h
//  KickCPP
//
//  Created by morten on 8/25/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <vector>
#include <map>
#include "kick/core/project_asset.h"
#include "kick/core/event_listener.h"
#include "kick/core/kickgl.h"
#include "kick/texture/texture2d.h"
#include "kick/texture/texture_cube.h"
#include "shader_enums.h"

namespace kick {
    
    class Shader;
    
    struct MaterialData {
        MaterialData(int intValue):value{intValue}, glType{GL_INT} {}
        MaterialData(float floatValue):value{floatValue}, glType{GL_FLOAT}{}
        MaterialData(glm::vec4 vec4Value):value{vec4Value}, glType{GL_FLOAT_VEC4}{}
        MaterialData(glm::mat3 mat3Value):value{mat3Value}, glType{GL_FLOAT_MAT3}{}
        MaterialData(glm::mat4 mat4Value):value{mat4Value}, glType{GL_FLOAT_MAT4}{}
        MaterialData(Texture2D *textureValue):value{textureValue}, glType{GL_SAMPLER_2D}{}
        MaterialData(TextureCube *textureValue):value{textureValue}, glType{GL_SAMPLER_CUBE}{}

        MaterialData(const MaterialData & val);
        MaterialData& operator=(const MaterialData& other);

        union ValueType{
            // union constructors
            ValueType(int intValue):intValue{intValue}{}
            ValueType(float floatValue):floatValue{floatValue}{}
            ValueType(glm::vec4 vec4Value):vec4Value{vec4Value}{}
            ValueType(glm::mat3 mat3Value):mat3Value{mat3Value}{}
            ValueType(glm::mat4 mat4Value):mat4Value{mat4Value}{}
            ValueType(Texture2D* texture2D):texture2D{texture2D}{}
            ValueType(TextureCube* textureCube):textureCube{textureCube}{}
            int intValue;
            float floatValue;
            glm::vec4 vec4Value;
            glm::mat3 mat3Value;
            glm::mat4 mat4Value;
            Texture2D* texture2D;
            TextureCube* textureCube;
        } value;
        int shaderLocation;
        int glType;
        // is the value inherited from the default uniform value in the shader
        bool defaultUniform = false;
    };
    
    class Material : public ProjectAsset {
    public:
        explicit Material(Project *p, std::shared_ptr<Shader> shader = {});
        ~Material();
        void setShader(std::shared_ptr<Shader> shader);
        std::shared_ptr<Shader> getShader();
        void setUniform(std::string name, int value);
        void setUniform(std::string name, float value);
        void setUniform(std::string name, glm::vec4 value);
        void setUniform(std::string name, glm::mat3 value);
        void setUniform(std::string name, glm::mat4 value);
        void setUniform(std::string name, std::shared_ptr<Texture2D> value);
        void setUniform(std::string name, std::shared_ptr<TextureCube> value);

        int bind();
        int getRenderOrder();
    private:
        template <class E>
        void setUniformInternal(std::string name, E value);
        void shaderChanged(ShaderEvent se);
        void setDefaultUniforms();
        void updateShaderLocation(std::string name, MaterialData& value);
        void setUniformData(std::string name, MaterialData&& value);
        // current data (may misfit with current shader)
        std::map<std::string, MaterialData> currentUniformData;
        std::shared_ptr<Shader> shader;
        EventListener<ShaderEvent> shaderChangedListener;
        int renderOrder = 1000;
        std::map<std::string, std::shared_ptr<Texture2D>> texture2DRef;
        std::map<std::string, std::shared_ptr<TextureCube>> textureCubeRef;
    };
    
    std::string to_string(MaterialData & data);

    template <class E>
    void Material::setUniformInternal(std::string name, E value){
        MaterialData data{value};
        setUniformData(name, std::move(data));
    }
}