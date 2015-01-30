//
//  material.h
//  KickCPP
//
//  Created by morten on 8/25/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once

#include <iostream>
#include <memory>

#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <glm/gtc/type_ptr.hpp>
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

        union MaterialValueType{ // todo find workaround should be union
            // union constructors
            MaterialValueType(int intValue):intValue{intValue}{}
            MaterialValueType(float floatValue){this->floatValue[0] = floatValue;}
            MaterialValueType(glm::vec4 vec4Value){ for (int i=0;i<4;i++){floatValue[i] = vec4Value[i]; }}
            MaterialValueType(glm::mat3 mat3Value){ for (int i=0;i<9;i++){floatValue[i] = glm::value_ptr(mat3Value)[i]; }}
            MaterialValueType(glm::mat4 mat4Value){ for (int i=0;i<16;i++){floatValue[i] = glm::value_ptr(mat4Value)[i]; }}
            MaterialValueType(Texture2D* texture2d):texture2D{texture2d}{}
            MaterialValueType(TextureCube* textureCube):textureCube{textureCube}{}
            int intValue;
            float floatValue[16];
            Texture2D* texture2D;
            TextureCube* textureCube;
        } value;

        void setValue(MaterialValueType val);

        int shaderLocation;
        int glType;
        // is the value inherited from the default uniform value in the shader
        bool defaultUniform = false;
    };
    
    class Material : public ProjectAsset {
    public:
        explicit Material(std::shared_ptr<Shader> shader = {});
        Material(const Material& copy);
        ~Material();
        void setShader(std::shared_ptr<Shader> shader);
        std::shared_ptr<Shader> shader();
        void setUniform(std::string name, int value);
        void setUniform(std::string name, float value);
        void setUniform(std::string name, glm::vec4 value);
        void setUniform(std::string name, glm::mat3 value);
        void setUniform(std::string name, glm::mat4 value);
        void setUniform(std::string name, std::shared_ptr<Texture2D> value);
        void setUniform(std::string name, std::shared_ptr<TextureCube> value);

        int bind();
        int renderOrder();
    private:
        template <class E>
        void setUniformInternal(std::string name, E value);
        void shaderChanged(ShaderEvent se);
        void setDefaultUniforms();
        void updateShaderLocation(std::string name, MaterialData& value);
        void setUniformData(std::string name, MaterialData&& value);
        // current data (may misfit with current shader)
        std::map<std::string, MaterialData> currentUniformData;
        std::shared_ptr<Shader> mShader;
        EventListener<ShaderEvent> shaderChangedListener;
        int mRenderOrder = 1000;
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