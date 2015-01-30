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
#include "kick/core/debug.h"

using namespace std;

namespace kick {
    MaterialData::MaterialData(const MaterialData & val)
    :shaderLocation(val.shaderLocation),
    glType(val.glType),
    value(0),
    defaultUniform(val.defaultUniform)
    {
        setValue(val.value);
    }

    MaterialData& MaterialData::operator=(const MaterialData& other){
        shaderLocation = other.shaderLocation;
        glType = other.glType;
        setValue(other.value);
        defaultUniform = other.defaultUniform;
        return *this;
    }

    void MaterialData::setValue(MaterialValueType val) {
        for (int i=0;i<16;i++){
            value.floatValue[i] = val.floatValue[i];
        }
    }
    
    Material::Material(std::shared_ptr<Shader> s)
    {
        setShader(s);
    }

    Material::Material(const Material& copy)
            :currentUniformData(copy.currentUniformData),
             mShader(nullptr),
             shaderChangedListener(),
             mRenderOrder(copy.mRenderOrder),
             texture2DRef(copy.texture2DRef),
             textureCubeRef(copy.textureCubeRef)
    {
        setShader(copy.mShader);
    }


    Material::~Material(){
    }
    
    void Material::setShader(std::shared_ptr<Shader> shader){
        if (this->mShader == shader) return;
        this->mShader = shader;
        if (shader){
            using namespace std::placeholders;
            auto f = std::bind(&Material::shaderChanged, this, _1);
            shaderChangedListener = shader->shaderChanged.createListener(f);
            shaderChanged({shader.get(), ShaderEventType::all });
            setDefaultUniforms();
        } else {
            shaderChangedListener = EventListener<ShaderEvent>();
        }
    }

    std::shared_ptr<Shader> Material::shader(){
        return mShader;
    }
        
    void Material::shaderChanged(ShaderEvent se){
        if (se.eventType == ShaderEventType::all || se.eventType == ShaderEventType::shader) {
            for (auto &pair : currentUniformData) {
                updateShaderLocation(pair.first, pair.second);
            }
            mRenderOrder = mShader ? mShader->getRenderOrder() : 0;
        }
        if (se.eventType == ShaderEventType::all || se.eventType == ShaderEventType::defaultUniform) {
            setDefaultUniforms();
        }
    }
    
    void Material::updateShaderLocation(std::string name, MaterialData& value){
        if (mShader == nullptr){
            value.shaderLocation = -1;
            return;
        }
        auto descriptor = mShader->getShaderUniform(name);
        if (descriptor == nullptr){
            value.shaderLocation = -1;
        } else {
            value.shaderLocation = descriptor->index;
        }
    }
    
    void Material::setUniformData(std::string name, MaterialData&& value){
        auto pos = currentUniformData.find(name);
        if (pos != currentUniformData.end()){
            pos->second.setValue(value.value);
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
                    glUniform1f(location, value.floatValue[0]);
                    break;
                case GL_FLOAT_MAT2:
                    //glUniformMatrix2fv(location, false, value.mat2Value);
//                    throw invalid_argument();
                    logError("GL_FLOAT_MAT2 is currently unsupported");
                    break;
                case GL_FLOAT_MAT3:
                    glUniformMatrix3fv(location, 1, GL_FALSE, value.floatValue);
                    break;
                case GL_FLOAT_MAT4:
                    glUniformMatrix4fv(location, 1, GL_FALSE, value.floatValue);
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
                    glUniform4fv(location, 1, value.floatValue);
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
                res = res + "GL_FLOAT value: "+std::to_string(data.value.floatValue[0]);
                break;
            case GL_FLOAT_VEC4:
                res = res + "GL_FLOAT_VEC4 value: ";
                for (int i=0;i<4;i++){
                    res += std::to_string(data.value.floatValue[i]);
                }
                break;
            case GL_FLOAT_MAT3:
                res = res + "GL_FLOAT_MAT3 value: ";
                for (int i=0;i<9;i++){
                    res += std::to_string(data.value.floatValue[i]);
                }
                break;
            case GL_FLOAT_MAT4:
                res = res + "GL_FLOAT_MAT4 value: ";
                for (int i=0;i<16;i++){
                    res += std::to_string(data.value.floatValue[i]);
                }
                break;
            default:break;
                res = res + "unknown type";
                break;
        }
        return res;
    }

    void Material::setDefaultUniforms() {
        vector<string> unmappedOrDefaultUniforms;
        for (auto& u : mShader->getShaderUniforms()){
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
            if (mShader->tryGetDefaultUniform(name, mat)){
                setUniformData(name, move(mat));
            }
        }
    }

    int Material::renderOrder() {
        return mRenderOrder;
    }

    void Material::setUniform(std::string name, int value) {
        setUniformInternal(name, value);
    }

    void Material::setUniform(std::string name, float value) {
        setUniformInternal(name, value);
    }

    void Material::setUniform(std::string name, glm::vec4 value) {
        setUniformInternal(name, value);
    }

    void Material::setUniform(std::string name, glm::mat3 value) {
        setUniformInternal(name, value);
    }

    void Material::setUniform(std::string name, glm::mat4 value) {
        setUniformInternal(name, value);
    }

    void Material::setUniform(std::string name, std::shared_ptr<Texture2D> value) {
        texture2DRef[name] = value;
        setUniformInternal(name, value.get());
    }

    void Material::setUniform(std::string name, std::shared_ptr<TextureCube> value) {
        textureCubeRef[name] = value;
        setUniformInternal(name, value.get());
    }
}