//
//  shader.cpp
//  KickCPP
//
//  Created by morten on 8/25/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/material/shader.h"
#include "kick/material/material.h"
#include "kick/core/Project.h"
#include "kick/core/Engine.h"
#include <sstream>
#include <vector>
#include <iostream>
#include <array>
#include <cassert>
#include <regex>
#include <cstring>
#include <algorithm>
#include <iterator>
#include "kick/core/cpp_ext.h"
#include "kick/scene/transform.h"

using namespace std;

namespace kick {
    
#define casetype(X) case static_cast<int>(X): \
enumValue = X; \
break;
    
    
    
    bool toEnum(int value, ShaderType& enumValue){
        switch (value) {
                casetype(ShaderType::VertexShader);
                casetype(ShaderType::FragmentShader);
                casetype(ShaderType::GeometryShader);
            default:
                return false;
        }
        return true;
    }
    
    bool toEnum(int value, ShaderErrorType& enumValue){
        switch (value) {
                casetype(ShaderErrorType::VertexShader);
                casetype(ShaderErrorType::FragmentShader);
                casetype(ShaderErrorType::GeometryShader);
                casetype(ShaderErrorType::Linker);
                casetype(ShaderErrorType::IncompleteShader);
            default:
                return false;
        }
        return true;
    }
    
    bool toEnum(int value, BlendType& enumValue){
        switch (value) {
                casetype(BlendType::Zero);
                casetype(BlendType::One);
                casetype(BlendType::SrcColor);
                casetype(BlendType::OneMinusSrcColor);
                casetype(BlendType::DstColor);
                casetype(BlendType::OneMinusDstColor);
                casetype(BlendType::SrcAlpha);
                casetype(BlendType::OneMinusSrcAlpha);
                casetype(BlendType::DstAlpha);
                casetype(BlendType::OneMinusDstAlpha);
                casetype(BlendType::ConstantColor);
                casetype(BlendType::OneMinusConstantColor);
                casetype(BlendType::ConstantAlpha);
                casetype(BlendType::OneMinusConstantAlpha);
            default:
                return false;
        }
        return true;
    }
    
    bool toEnum(int value, FaceCullingType& enumValue){
        switch (value) {
                casetype(FaceCullingType::Front);
                casetype(FaceCullingType::Back);
                casetype(FaceCullingType::FrontAndBack);
                casetype(FaceCullingType::None);
            default:
                return false;
        }
        return true;
    }
    
    bool toEnum(int value, ZTestType& enumValue){
        switch (value) {
                casetype(ZTestType::Never);
                casetype(ZTestType::Less);
                casetype(ZTestType::Equal);
                casetype(ZTestType::LEqual);
                casetype(ZTestType::Greater);
                casetype(ZTestType::NotEqual);
                casetype(ZTestType::GEqual);
                casetype(ZTestType::Always);
            default:
                return false;
        }
        return true;
    }
    
    ShaderObj::ShaderObj(ShaderType type)
    :shader(glCreateShader((GLenum)type))
    {
        assert(shader != 0);
        assert(shader != GL_INVALID_ENUM);
    }
    
    ShaderObj::ShaderObj(ShaderObj&& other)
    :shader(other.shader){
        other.shader = 0;
    }
    
    ShaderObj& ShaderObj::operator=(ShaderObj&& other){
        if (this != &other){
            glDeleteShader(shader);
            shader = other.shader;
            other.shader = 0;
        }
        return *this;
    }

    ShaderObj::~ShaderObj(){
        if (shader>0){
            glDeleteShader(shader);
        }
    }

    void ShaderObj::detach(GLuint shaderProgram){
        // warn http://stackoverflow.com/a/9117411/420250
        glDetachShader(shaderProgram, shader);
    }

    Shader::Shader(Shader&& s)
    :ProjectAsset(s.project)
    {
        *this = move(s);
    }
    
    Shader& Shader::operator=(Shader&& o){
        swap(shaderProgram, o.shaderProgram);
        swap(shaderSources, o.shaderSources);
        swap(outputAttributeName, o.outputAttributeName);
        swap(shaderAttributes, o.shaderAttributes);
        swap(shaderUniforms, o.shaderUniforms);
        swap(blendDFactorAlpha, o.blendDFactorAlpha);
        swap(blendDFactorRGB, o.blendDFactorRGB);
        swap(blendSFactorAlpha, o.blendSFactorAlpha);
        swap(blendSFactorRGB, o.blendSFactorRGB);
        swap(depthMask, o.depthMask);
        swap(faceCulling, o.faceCulling);
        swap(polygonOffsetEnabled, o.polygonOffsetEnabled);
        swap(polygonOffsetFactorAndUnit, o.polygonOffsetFactorAndUnit);
        swap(zTest, o.zTest);
        swap(project, o.project);
        return *this;
    }
    
    Shader::Shader(Project *p, string vertexShader, string fragmentShader, string geometryShader)
    :ProjectAsset{p},shaderProgram(0)
    {
        setShaderSource(ShaderType::VertexShader, vertexShader);
        setShaderSource(ShaderType::FragmentShader, fragmentShader);
        setShaderSource(ShaderType::GeometryShader, geometryShader);
        if (vertexShader.length()>0 && fragmentShader.length()>0) {
            apply();
        }
    }
    
    Shader::~Shader(){
        if (shaderProgram != 0){
            glDeleteProgram(shaderProgram);
        }
    }
    
    const UniformDescriptor* Shader::getShaderUniform(std::string name) const{
        for (auto & desc : shaderUniforms) {
            if (desc.name == name){
                return &desc;
            }
        }
        return nullptr;
    }
    
    vector<AttributeDescriptor > getActiveShaderAttributes(GLuint programid){
        vector<AttributeDescriptor > res;
        int numberOfActiveAttributes;
        glGetProgramiv(programid,GL_ACTIVE_ATTRIBUTES,&numberOfActiveAttributes);
        GLint bufSize;
        glGetProgramiv(programid, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &bufSize);
        vector<GLchar> buffer(bufSize);
        for (GLuint i=0;i<numberOfActiveAttributes;i++){
            AttributeDescriptor  att{i};
            glGetActiveAttrib(programid,
                              i,
                              bufSize,
                              nullptr,
                              &att.size,
                              &att.type,
                              &buffer[0]);
            att.name = &buffer[0];
            att.semantic = to_semantic(att.name);
            if (att.semantic == VertexAttributeSemantic::Unknown){
                throw invalid_argument(string{"Invalid vertex attribute in shader source: "}+att.name);
            }
            res.push_back(att);
        }
        
        return move(res);
    }
    
    vector<UniformDescriptor > getActiveShaderUniforms(GLuint programid){
        vector<UniformDescriptor > res;
        
        int numberOfActiveUniforms;
        glGetProgramiv(programid,GL_ACTIVE_UNIFORMS,&numberOfActiveUniforms);
        GLint bufSize;
        glGetProgramiv(programid, GL_ACTIVE_UNIFORM_MAX_LENGTH, &bufSize);
        vector<GLchar> buffer(bufSize);
        for (GLuint i=0;i<numberOfActiveUniforms;i++){
            UniformDescriptor  uni{i};
            glGetActiveUniform(programid,
                               i,
                               bufSize,
                               nullptr,
                               &uni.size,
                               &uni.type,
                               &buffer[0]);
            uni.name = &buffer[0];
            uni.index = glGetUniformLocation(programid, &buffer[0]);
            res.push_back(uni);
        }

        return move(res);
    }
    
    string Shader::getPrecompiledSource(string source){
        if (source.find("#version") != 0){
            string prefix = "#version 150\n";// context.getGLSLPrefix() // todo - get prefix from context
            source = prefix + source;
        }
        
        // remove commented out usages of #pragma include
        static regex regExpSearch { R"(//\s*#?\s*pragma\s+include[^\n]*)"};
        source = regex_replace(source, regExpSearch, "");

        // find pragma include files
        static regex regExpSearch2 { "[ ]*#\\s*pragma\\s+include\\s+\"([^\"]*)\"[^\\n]*"};
        vector<string> includeNames;
        auto iter = std::sregex_token_iterator(source.begin(), source.end(), regExpSearch2, 1);
        auto endIter = std::sregex_token_iterator();
        while (iter != endIter){
            includeNames.push_back(*iter);
            iter++;
        }
        
        // replace source
        for (auto includeName : includeNames){
            string includedSource = Project::loadTextResource(includeName);
            if (includedSource.size() == 0){
                cout << "Could not load source "<<includeName<<endl;
            } else {
                regex regExpSearch3 { string{"[ ]*#\\s*pragma\\s+include\\s+\""}+includeName+"\"[^\\n]*"};
                source = regex_replace(source, regExpSearch3, includedSource);
            }
        }
        
        // Insert compile
        size_t indexOfNewline = source.find('\n');
        string version = source.substr(0, indexOfNewline); // save version info
        string sourcecode = source.substr(indexOfNewline + 1); // strip version info
        
        return version + "\n" +
        "#define SHADOWS " + (Engine::instance->config.shadows == true?"true":"false") + "\n" +
        "#define LIGHTS " + std::to_string((int)Engine::instance->config.maxNumerOfLights) + "\n" +
        "#line " + std::to_string(2) + "\n" +
            sourcecode;
    }
    
    void Shader::apply(){
        if (shaderProgram){
            glDeleteShader(shaderProgram);
        }
        shaderProgram = glCreateProgram();
        vector<ShaderObj> shaderObjects;
        vector<ShaderType> requiredTypes{ShaderType::VertexShader, ShaderType::FragmentShader};
        for (auto & element : shaderSources) {
            auto shaderType = element.first;
            string & source = element.second;
            if (source.length()>0){
                string precompiledSource = getPrecompiledSource(source);
                
                shaderObjects.push_back(compileShader(precompiledSource, shaderType));
                // remove element from vector
                requiredTypes.erase(std::remove(requiredTypes.begin(), requiredTypes.end(), shaderType), requiredTypes.end());
                glAttachShader( shaderProgram,  shaderObjects.back());
            }
        }
        if (requiredTypes.size()>0){
            throw ShaderBuildException{"Both vertex and fragment shader required.", ShaderErrorType::IncompleteShader};
        }
        
        linkProgram();
        // shaderObjects deleted when goes out of scope (which is ok as long as program is not deleted)
        glUseProgram(shaderProgram);

        // clean up
        for (auto & shaderObject : shaderObjects){
            shaderObject.detach(shaderProgram);
        }
        shaderObjects.clear();

        shaderAttributes = getActiveShaderAttributes(shaderProgram);
        shaderUniforms = getActiveShaderUniforms(shaderProgram);
        
        shaderProgramChanged.notifyListeners(this);

    }
    
    void Shader::linkProgram(){
        glBindFragDataLocation(shaderProgram, 0, outputAttributeName.c_str());
        
		glLinkProgram(shaderProgram);
        
		GLint  linked;
		glGetProgramiv( shaderProgram, GL_LINK_STATUS, &linked );
		if (linked == GL_FALSE) {
			GLint  logSize;
			glGetProgramiv( shaderProgram, GL_INFO_LOG_LENGTH, &logSize);
            vector<char> errorLog((size_t) logSize);
			glGetProgramInfoLog( shaderProgram, logSize, NULL, &errorLog[0] );
            throw ShaderBuildException{&errorLog[0], ShaderErrorType::Linker};
		}
    }
    
    void updateFaceCulling(FaceCullingType faceCulling){
        if (faceCulling == FaceCullingType::None){
            glDisable(GL_CULL_FACE);
        } else {
            glEnable(GL_CULL_FACE);
            glCullFace(static_cast<GLuint>(faceCulling));
        }
    }
    
    void updateDepthProperties(ZTestType zTest, bool depthMask){
        if (zTest == ZTestType::Never){
            glDisable(GL_DEPTH_TEST);
        } else {
            glEnable(GL_DEPTH_TEST);
        }
        glDepthFunc(static_cast<GLuint>(zTest));
        glDepthMask((GLboolean) depthMask);
    }
    
    void updateBlending(bool blend, BlendType blendDFactorAlpha,BlendType blendDFactorRGB, BlendType blendSFactorAlpha, BlendType blendSFactorRGB){
        if (blend){
            glEnable(GL_BLEND);
            GLenum sfactorRGB = static_cast<GLenum>(blendSFactorRGB);
            GLenum dfactorRGB = static_cast<GLenum>(blendDFactorRGB);
            GLenum sfactorAlpha = static_cast<GLenum>(blendSFactorAlpha);
            GLenum dfactorAlpha = static_cast<GLenum>(blendDFactorAlpha);;
            glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
        } else {
            glDisable(GL_BLEND);
        }
    }
    
    void Shader::bind(){
        glUseProgram(shaderProgram);
        updateFaceCulling(faceCulling);
        updateDepthProperties(zTest, depthMask);
        updateBlending(blend, blendDFactorAlpha, blendDFactorRGB, blendSFactorAlpha, blendSFactorRGB);
    }
    
    ShaderObj Shader::compileShader(std::string source, ShaderType type){
        ShaderObj shader(type);
        const GLchar* sourceArray[1];
        sourceArray[0] = source.c_str();
        GLint stringLengths[1];
        stringLengths[0] = (GLint)source.length();
        glShaderSource(shader, 1, sourceArray, stringLengths);
        glCompileShader(shader);
        GLint success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE){
            ShaderBuildException::throwCurrentCompileException(shader, (ShaderErrorType)type, source);
        }
        return move(shader);
    }
    
    void Shader::setShaderSource(ShaderType type, string str){
        shaderSources[type] = str;
    }
    
    std::string Shader::getShaderSource(ShaderType type) const {
        return shaderSources.at(type);
    }
    
    const std::vector<AttributeDescriptor >& Shader::getShaderAttributes() const  {
        return shaderAttributes;
    }
    
    const std::vector<UniformDescriptor >& Shader::getShaderUniforms() const {
        return shaderUniforms;
    }
    
    const AttributeDescriptor* Shader::getShaderAttribute(VertexAttributeSemantic semantic) const{
        for (int i=0;i<shaderAttributes.size();i++){
            if (shaderAttributes[i].semantic == semantic){
                return &shaderAttributes[i];
            }
        }
        return nullptr;
    }
    
    string ShaderBuildException::extractLines(std::string errorMessage, std::string source, int extraLines){
        static regex regExpSearch { R"(\d+:(\d+))"};
        auto iter = std::sregex_token_iterator(errorMessage.begin(), errorMessage.end(), regExpSearch, 1);
        auto endIter = std::sregex_token_iterator();
        if (iter != endIter){
            string lineStr = *iter;
            int line = atoi(lineStr.c_str());
            constexpr int buffer_size = 512;
            array<char, buffer_size> lineBuffer;
            istringstream inbuf(source);
            string res;
            for (int i=1;i<=line+extraLines && inbuf.getline(&lineBuffer[0],buffer_size);i++){
                if (strncmp(&lineBuffer[0], "#line ", 6)==0){
                    string s{&lineBuffer[0]};
                    static regex regExpSearchLine { R"((\d+))"};
                    auto iterLine = std::sregex_token_iterator(s.begin(), s.end(), regExpSearchLine, 1);
                    if (iterLine != endIter){
                        string newLineNumberStr = *iterLine;
                        i = atoi(newLineNumberStr.c_str());
                    }
                }
                if (i >= line-extraLines){
                    res += std::to_string(i)+"\t"+string{&lineBuffer[0]}+"\n";
                }
            }
            return res;
        } else
            return "";
    }
    
    
    void ShaderBuildException::throwCurrentCompileException(GLuint shader, ShaderErrorType type, string source){
        GLint logSize = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
        
        vector<char> errorLog((unsigned long) logSize);
        glGetShaderInfoLog(shader, logSize, &logSize, &errorLog[0]);
        string errorLines = extractLines(&errorLog[0], source);
        throw ShaderBuildException{&errorLog[0], type, errorLines};
    }
    
    void Shader::setBlend(bool b){ blend = b; }
    bool Shader::getBlend() { return blend; }
    BlendType Shader::getBlendDFactorAlpha() { return blendDFactorAlpha; }
    BlendType Shader::getBlendDFactorRGB() { return blendDFactorRGB; }
    BlendType Shader::getBlendSFactorAlpha() { return blendSFactorAlpha; }
    BlendType Shader::getBlendSFactorRGB() { return blendSFactorRGB; }
    void Shader::setBlendDFactorAlpha(BlendType blendDFactorAlpha) { this->blendDFactorAlpha=blendDFactorAlpha; }
    void Shader::setBlendDFactorRGB(BlendType blendDFactorRGB) { this->blendDFactorRGB=blendDFactorRGB; }
    void Shader::setBlendSFactorAlpha(BlendType blendSFactorAlpha) { this->blendSFactorAlpha = blendSFactorAlpha; }
    void Shader::setBlendSFactorRGB(BlendType blendSFactorRGB) { this->blendSFactorRGB = blendSFactorRGB; }
    void Shader::setDepthMask(bool depthMask) { this->depthMask = depthMask;}
    bool Shader::getDepthMash() { return depthMask; }
    void Shader::setFaceCulling(FaceCullingType faceCulling) { this->faceCulling = faceCulling; }
    FaceCullingType Shader::getFaceCulling() { return faceCulling; }
    void Shader::setPolygonOffsetEnabled(bool polygonOffsetEnabled) { this->polygonOffsetEnabled = polygonOffsetEnabled; }
    bool Shader::getPolygonOffsetEnabled() { return polygonOffsetEnabled; }
    void Shader::setPolygonOffsetFactorAndUnit(glm::vec2 polygonOffsetFactorAndUnit) { this->polygonOffsetFactorAndUnit = polygonOffsetFactorAndUnit; }
    glm::vec2 Shader::getPolygonOffsetFactorAndUnit() { return polygonOffsetFactorAndUnit; }
    void Shader::setZTest(ZTestType zTest) { this->zTest = zTest; }
    ZTestType Shader::getZTest() { return zTest; }
    
    void Shader::bind_uniforms(Material *material, EngineUniforms *engineUniforms, Transform* transform){
        material->bind();
        
        for (auto& uniform : shaderUniforms){
            if (uniform.name == UniformNames::modelMatrix){
                auto globalMatrix = transform->getGlobalMatrix();
                glUniformMatrix4fv(uniform.index, 1, GL_FALSE, glm::value_ptr(globalMatrix));
            } else if (uniform.name == UniformNames::mv){
                auto modelView = engineUniforms->viewMatrix * transform->getGlobalMatrix();
                glUniformMatrix4fv(uniform.index, 1, GL_FALSE, glm::value_ptr(modelView));
            } else if (uniform.name == UniformNames::norm) {
                auto modelView = engineUniforms->viewMatrix * transform->getGlobalMatrix();
                auto normal = glm::inverseTranspose(glm::mat3(modelView));
                glUniformMatrix3fv(uniform.index, 1, GL_FALSE, glm::value_ptr(normal));
            } else if (uniform.name == UniformNames::v) {
                auto viewMatrix = engineUniforms->viewMatrix;
                glUniformMatrix4fv(uniform.index, 1, GL_FALSE, glm::value_ptr(viewMatrix));
            } else if (uniform.name == UniformNames::worldCamPos){
                auto cameraPos = engineUniforms->currentCameraTransform->getPosition();
                glUniform3fv(uniform.index, 1, glm::value_ptr(cameraPos));
            } else if (uniform.name == UniformNames::world2object){
                auto world2object = transform->getGlobalTRSInverse();
                glUniformMatrix4fv(uniform.index, 1, false, glm::value_ptr(world2object));
            } else if (uniform.name == UniformNames::mvProj){
                auto mvProj = engineUniforms->viewProjectionMatrix * transform->getGlobalMatrix();
                glUniformMatrix4fv(uniform.index, 1, false, glm::value_ptr(mvProj));
            } else if (uniform.name == UniformNames::gameObjectUID){
                throw invalid_argument("gameObjectUID not yet implemented"); // todo
            } else if (uniform.name == UniformNames::shadowMapTexture) {
                //throw invalid_argument("shadowMapTexture not yet implemented"); // todo
                cout <<"shadowMapTexture not yet implemented\n"; // todo
            } else if (uniform.name == UniformNames::lightMat){
                auto lightMatrix = engineUniforms->lightMatrix * transform->getGlobalMatrix();
                glUniformMatrix4fv(uniform.index, 1, false, glm::value_ptr(lightMatrix));
            }
            // debug output
            //else {
            //    continue;
            //}
            //cout << to_string(uniform)<<"\n";
        }
    }
    
    string to_string(AttributeDescriptor attDesc){
        string res = "AttributeDescriptor {";
        res = res + " Index: "+std::to_string(attDesc.index)+
         res + " Size: "+std::to_string(attDesc.size)+
         res + " Type: "+std::to_string(attDesc.type)+
         res + " Name: "+attDesc.name+
         res + " Semantic: "+to_string(attDesc.semantic)+
        " }";
        return res;
    }
    
    string to_string(UniformDescriptor uniformDesc){
        string res = "UniformDescriptor {";
        res = res + " Index: "+std::to_string(uniformDesc.index)+
         res + " Size: "+std::to_string(uniformDesc.size)+
         res + " Type: "+std::to_string(uniformDesc.type)+
         res + " Name: "+uniformDesc.name+
        " }";
        return res;
    }

}