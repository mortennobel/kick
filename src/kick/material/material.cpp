//
//  material.cpp
//  KickCPP
//
//  Created by morten on 8/25/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/material/material.h"

#include <functional>
#include <utility>
#include <string>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "kick/material/shader.h"
#include "kick/core/log.h"

using namespace std;

namespace kick {
    MaterialData::MaterialData(const MaterialData & val)
    :shaderLocation(val.shaderLocation),
    glType(val.glType),
    value(0),
    defaultUniform(val.defaultUniform)
    {
        value.mat4Value = val.value.mat4Value; // if this safe?
    }

    MaterialData& MaterialData::operator=(const MaterialData& other){
        shaderLocation = other.shaderLocation;
        glType = other.glType;
        value.mat4Value = other.value.mat4Value; // copy largest element
        defaultUniform = other.defaultUniform;
        return *this;
    }
    
    Material::Material(Project *p, Shader* s)
    :ProjectAsset{p}
    {
        setShader(s);
    }
    
    Material::~Material(){
    }
    
    void Material::setShader(Shader *shader){
        if (this->shader == shader) return;
        this->shader = shader;
        if (shader){
            using namespace std::placeholders;
            auto f = std::bind(&Material::shaderChanged, this, _1);
            shaderChangedListener = shader->shaderChanged.createListener(f);
            shaderChanged({shader, ShaderEventType::all });
            setDefaultUniforms();
        } else {
            shaderChangedListener = EventListener<ShaderEvent>();
        }
    }
    
    Shader* Material::getShader(){
        return shader;
    }
        
    void Material::shaderChanged(ShaderEvent se){
        if (se.eventType == ShaderEventType::all || se.eventType == ShaderEventType::shader) {
            for (auto &pair : currentUniformData) {
                updateShaderLocation(pair.first, pair.second);
            }
        }
        if (se.eventType == ShaderEventType::all || se.eventType == ShaderEventType::defaultUniform) {
            setDefaultUniforms();
        }

    }
    
    void Material::updateShaderLocation(std::string name, MaterialData& value){
        if (shader == nullptr){
            value.shaderLocation = -1;
            return;
        }
        auto descriptor = shader->getShaderUniform(name);
        if (descriptor == nullptr){
            value.shaderLocation = -1;
        } else {
            value.shaderLocation = descriptor->index;
        }
    }
    
    void Material::setUniformData(std::string name, MaterialData&& value){
        auto pos = currentUniformData.find(name);
        if (pos != currentUniformData.end()){
            pos->second.value.mat4Value = value.value.mat4Value; // copy largest element
            if (pos->second.glType != value.glType){
                pos->second.glType = value.glType;
                updateShaderLocation(name, pos->second);
            }
        } else {
            // not found insert new
            auto insertedElement = currentUniformData.emplace(make_pair(name, move(value)));
            bool didInsert = insertedElement.second;
            assert(didInsert);
            auto insertedIterator = insertedElement.first;
            updateShaderLocation(name, insertedIterator->second);
        }
    }
    
    int Material::bind(){
        shader->bind();// should not be needed
        int currentTexture = 0;
        for (auto & uniform : currentUniformData) {
            auto & materialData = uniform.second;
            if (materialData.shaderLocation == -1){
                continue;
            }
            GLint location = materialData.shaderLocation;
            auto & value = materialData.value;
            switch (materialData.glType) {
                case GL_FLOAT:
                    glUniform1f(location, value.floatValue);
                    break;
                case GL_FLOAT_MAT2:
                    //glUniformMatrix2fv(location, false, value.mat2Value);
//                    throw invalid_argument();
                    logError("GL_FLOAT_MAT2 is currently unsupported");
                    break;
                case GL_FLOAT_MAT3:
                    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value.mat3Value));
                    break;
                case GL_FLOAT_MAT4:
                    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value.mat4Value));
                    break;
                case GL_FLOAT_VEC2:
                    //glUniform2fv(location, value.vec4Value);
//                    throw invalid_argument("GL_FLOAT_VEC2 is currently unsupported");
                    logError("GL_FLOAT_VEC2 is currently unsupported");
                    break;
                case GL_FLOAT_VEC3:
//                    throw invalid_argument("GL_FLOAT_VEC3 is currently unsupported");
                    logError("GL_FLOAT_VEC3 is currently unsupported");
                    break;
                case GL_FLOAT_VEC4:
                    glUniform4fv(location, 1, glm::value_ptr(value.vec4Value));
                    break;
                case GL_INT:
                    glUniform1i(location, value.intValue);
                    break;
                case GL_INT_VEC2:
//                    throw invalid_argument("GL_INT_VEC2 is currently unsupported");
                    logError("GL_INT_VEC2 is currently unsupported");
                    break;
                case GL_INT_VEC3:
//                    throw invalid_argument("GL_INT_VEC3 is currently unsupported");
                    logError("GL_INT_VEC3 is currently unsupported");
                    break;
                case GL_INT_VEC4:
//                    throw invalid_argument("GL_INT_VEC4 is currently unsupported");
                    logError("GL_INT_VEC4 is currently unsupported");
                    break;
                case GL_SAMPLER_CUBE:
                    value.textureCube->bind(currentTexture);
                    glUniform1i(location, currentTexture);
                    currentTexture++;
                    break;
                case GL_SAMPLER_2D:
                    value.texture2D->bind(currentTexture);
                    glUniform1i(location, currentTexture);
                    currentTexture++;
                    break;
                default:
//                    throw invalid_argument("Unhandled type");
                    logError("Unhandled type");
            }
        }
        return currentTexture;
    }
    
    std::string to_string(MaterialData & data){
        auto res = std::string{"MaterialData{ shaderLocation: "}+std::to_string(data.shaderLocation)+" glType: ";
        switch (data.glType){
            case GL_INT:
                res = res + "GL_INT value: "+std::to_string(data.value.intValue);
                break;
            case GL_FLOAT:
                res = res + "GL_FLOAT value: "+std::to_string(data.value.floatValue);
                break;
            case GL_FLOAT_VEC4:
                res = res + "GL_FLOAT_VEC4 value: "+glm::to_string(data.value.vec4Value);
                break;
            case GL_FLOAT_MAT3:
                res = res + "GL_FLOAT_MAT3 value: "+glm::to_string(data.value.mat3Value);
                break;
            case GL_FLOAT_MAT4:
                res = res + "GL_FLOAT_MAT4 value: "+glm::to_string(data.value.mat4Value);
                break;
            default:break;
                res = res + "unknown type";
                break;
        }
        return res;
    }

    void Material::setDefaultUniforms() {
        vector<string> unmappedOrDefaultUniforms;
        for (auto& u : shader->getShaderUniforms()){
            bool isAutoMapped = u.name.length()>0 && u.name[0] == '_';
            if (isAutoMapped){
                continue;
            }
            auto currentValue = currentUniformData.find(u.name);
            bool isUnmapped = currentValue == currentUniformData.end();
            if (isUnmapped || currentValue->second.defaultUniform){
                unmappedOrDefaultUniforms.push_back(u.name);
            }
        }
        for (auto & name : unmappedOrDefaultUniforms){
            MaterialData mat{0};
            if (shader->tryGetDefaultUniform(name, mat)){
                setUniformData(name, move(mat));
            }
        }
    }
}