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
#include "kick/core/engine.h"
#include "kick/core/time.h"
#include "kick/math/misc.h"
#include <sstream>
#include <vector>
#include <iostream>
#include <array>
#include <cassert>
#include <regex>
#include <cstring>
#include <algorithm>
#include <iterator>
#include <sstream>
#include "kick/core/cpp_ext.h"
#include "kick/scene/transform.h"
#include "kick/scene/light.h"
#include "kick/core/log.h"
using namespace std;

namespace kick {
    
#define casetype(X) case static_cast<int>(X): \
enumValue = X; \
break;
    
    
    
    bool toEnum(int value, ShaderType& enumValue){
        switch (value) {
                casetype(ShaderType::VertexShader);
                casetype(ShaderType::FragmentShader);
#ifndef GL_ES_VERSION_2_0
                casetype(ShaderType::GeometryShader);
#endif
            default:
                return false;
        }
        return true;
    }
    
    bool toEnum(int value, ShaderErrorType& enumValue){
        switch (value) {
                casetype(ShaderErrorType::VertexShader);
                casetype(ShaderErrorType::FragmentShader);
#ifndef GL_ES_VERSION_2_0
                casetype(ShaderErrorType::GeometryShader);
#endif
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

    {
        *this = move(s);
    }
    
    Shader& Shader::operator=(Shader&& o){
        swap(mShaderProgram, o.mShaderProgram);
        swap(shaderSources, o.shaderSources);
        swap(outputAttributeName, o.outputAttributeName);
        swap(mShaderAttributes, o.mShaderAttributes);
        swap(shaderUniforms, o.shaderUniforms);
        swap(mBlendDFactorAlpha, o.mBlendDFactorAlpha);
        swap(mBlendDFactorRGB, o.mBlendDFactorRGB);
        swap(mBlendSFactorAlpha, o.mBlendSFactorAlpha);
        swap(mBlendSFactorRGB, o.mBlendSFactorRGB);
        swap(mDepthBufferWrite, o.mDepthBufferWrite);
        swap(mFaceCulling, o.mFaceCulling);
        swap(mPolygonOffsetEnabled, o.mPolygonOffsetEnabled);
        swap(mPolygonOffsetFactorAndUnit, o.mPolygonOffsetFactorAndUnit);
        swap(mZTest, o.mZTest);
        return *this;
    }
    
    Shader::Shader(string vertexShader, string fragmentShader, string geometryShader)
    : mShaderProgram(0)
    {
        setShaderSource(ShaderType::VertexShader, vertexShader);
        setShaderSource(ShaderType::FragmentShader, fragmentShader);
#ifndef GL_ES_VERSION_2_0
        setShaderSource(ShaderType::GeometryShader, geometryShader);
#endif
        if (vertexShader.length()>0 && fragmentShader.length()>0) {
            apply();
        }
    }
    
    Shader::~Shader(){
        if (mShaderProgram != 0){
            glDeleteProgram(mShaderProgram);
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
                              buffer.data());
            att.name = buffer.data();
            att.semantic = to_semantic(att.name);
            if (att.semantic == VertexAttributeSemantic::Unknown){
                logWarning(string{"Invalid vertex attribute in shader source: "}+att.name);
            }
            res.push_back(att);
        }
        
        return move(res);
    }

    void Shader::translateToGLSLES(string& source, ShaderType type){
        // replace textures
        if (type == ShaderType::VertexShader){
            static regex regExpSearchShim3 { R"(\n\s*out\b)"};
            source = regex_replace(source, regExpSearchShim3, "\nvarying");
            static regex regExpSearchShim4 { R"(\n\s*in\b)"};
            source = regex_replace(source, regExpSearchShim4, "\nattribute");
        } else {
            static regex regExpSearchShim2 { R"(\bfragColor\b)"};
            source = regex_replace(source, regExpSearchShim2, "gl_FragColor");
            static regex regExpSearchShim3 { R"(\n\s*out\b)"};
            source = regex_replace(source, regExpSearchShim3, "\n// out");
            static regex regExpSearchShim4 { R"(\n\s*in\b)"};
            source = regex_replace(source, regExpSearchShim4, "\nvarying");
        }

        static regex regExpSearchShim1 { R"(\s*uniform\s+sampler([\w]*)\s+([\w_]*)\s*;.*)"};
        istringstream iss(source);
        map<string,string> textureType;
        smatch match;
        for (std::string line; std::getline(iss, line); )
        {
            regex_search(line, match, regExpSearchShim1);
            if (match.size() > 0){
                string samplerType = match[1].str();
                string samplerName = match[2].str();
                textureType[samplerName] = samplerType;

            }
        }

        for (auto val : textureType){
            regex regExpSearchShim4 { string{"texture\\s*\\(\\s*"}+val.first+"\\s*," };
            source = regex_replace(source, regExpSearchShim4, string{"texture"}+val.second+"("+val.first+",");
        }
    }

    vector<UniformDescriptor > getActiveShaderUniforms(GLuint programid){
        vector<UniformDescriptor > res;
        
        int numberOfActiveUniforms;
        glGetProgramiv(programid,GL_ACTIVE_UNIFORMS,&numberOfActiveUniforms);
        GLint bufSize;
        glGetProgramiv(programid, GL_ACTIVE_UNIFORM_MAX_LENGTH, &bufSize);
        vector<GLchar> buffer(bufSize);
        for (GLuint i=0;i<numberOfActiveUniforms;i++){
            UniformDescriptor uni{i};
            glGetActiveUniform(programid,
                               i,
                               bufSize,
                               nullptr,
                               &uni.size,
                               &uni.type,
                               buffer.data());
            uni.name = buffer.data();
            int index = glGetUniformLocation(programid, buffer.data());
            if (index>=0){
                uni.index = index;
                res.push_back(uni);
            } else {
                cout << "cannot find uniform index "<< buffer.data()<<endl;
            }
        }

        return move(res);
    }
    
    string Shader::getPrecompiledSource(string source, ShaderType type) {
        if (source.find("#version") != 0){
#ifdef GL_ES_VERSION_2_0
            string prefix = "#version 100\n";// context.getGLSLPrefix() // todo - get prefix from context
#else
            string prefix = "#version 150\n";// context.getGLSLPrefix() // todo - get prefix from context
#endif
            source = prefix + source;
        }
        string precisionSpecifier = "";


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
            string includedSource;
            if (Project::loadTextResource(includeName, includedSource)) {
                if (includedSource.size() == 0) {
                    cout << "Could not load source " << includeName << endl;
                } else {
                    regex regExpSearch3{string{"[ ]*#\\s*pragma\\s+include\\s+\""} + includeName + "\"[^\\n]*"};
                    source = regex_replace(source, regExpSearch3, includedSource);
                }
            }
        }

#ifdef GL_ES_VERSION_2_0
        translateToGLSLES(source, type);
        if (type == ShaderType::FragmentShader){
            precisionSpecifier = "precision mediump float;\n";
        }
#endif
        // Insert compile
        size_t indexOfNewline = source.find('\n');
        string version = source.substr(0, indexOfNewline); // save version info
        source = source.substr(indexOfNewline + 1); // strip version info


        source = version + "\n" +
                precisionSpecifier+
        "#define SHADOWS " + (Engine::config().shadows?"true":"false") + "\n" +
        "#define LIGHTS " + std::to_string((int) Engine::config().maxNumerOfLights) + "\n" +
        "#line " + std::to_string(2) + "\n" +
                source;

        return source;
    }
    
    bool Shader::apply(){
        if (mShaderProgram){
            glDeleteShader(mShaderProgram);
        }
        mShaderProgram = glCreateProgram();
        vector<ShaderObj> shaderObjects;
        vector<ShaderType> requiredTypes{ShaderType::VertexShader, ShaderType::FragmentShader};
        for (auto & element : shaderSources) {
            auto shaderType = element.first;
            string & source = element.second;
            if (source.length()>0){
                string precompiledSource = getPrecompiledSource(source, shaderType);
                
                shaderObjects.push_back(compileShader(precompiledSource, shaderType));
                // remove element from vector
                requiredTypes.erase(std::remove(requiredTypes.begin(), requiredTypes.end(), shaderType), requiredTypes.end());
                glAttachShader(mShaderProgram,  shaderObjects.back());
            }
        }
        if (requiredTypes.size()>0){
            logError("Both vertex and fragment shader required.");
            return false;
        }
        
        bool linked = linkProgram();
        if (!linked){
            return false;
        }
        // shaderObjects deleted when goes out of scope (which is ok as long as program is not deleted)
        glUseProgram(mShaderProgram);

        mShaderAttributes = getActiveShaderAttributes(mShaderProgram);
        shaderUniforms = getActiveShaderUniforms(mShaderProgram);

        updateDefaultShaderLocation();

        shaderChanged.notifyListeners({this, ShaderEventType::shader });

        // clean up
        for (auto & shaderObject : shaderObjects){
            shaderObject.detach(mShaderProgram);
        }
        shaderObjects.clear();

        return true;
    }
    
    bool Shader::linkProgram(){
#ifndef GL_ES_VERSION_2_0
        glBindFragDataLocation(mShaderProgram, 0, outputAttributeName.c_str());
#endif
		glLinkProgram(mShaderProgram);
        
		GLint  linked;
		glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &linked );
		if (linked == GL_FALSE) {
			GLint  logSize;
			glGetProgramiv(mShaderProgram, GL_INFO_LOG_LENGTH, &logSize);
            vector<char> errorLog((size_t) logSize);
			glGetProgramInfoLog(mShaderProgram, logSize, NULL, errorLog.data() );
//            throw ShaderBuildException{, ShaderErrorType::Linker};
            logError(errorLog.data());
            return false;
		}
        return true;
    }
    
    void updateFaceCulling(FaceCullingType faceCulling){
        if (faceCulling == FaceCullingType::None){
            glDisable(GL_CULL_FACE);
        } else {
            glEnable(GL_CULL_FACE);
            glCullFace(static_cast<GLuint>(faceCulling));
        }
    }
    
    void updateDepthProperties(ZTestType zTest, bool depthWrite){
        if (zTest == ZTestType::Never){
            glDisable(GL_DEPTH_TEST);
        } else {
            glEnable(GL_DEPTH_TEST);
        }
        glDepthFunc(static_cast<GLuint>(zTest));
        glDepthMask(depthWrite ? GL_TRUE : GL_FALSE);
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
        glUseProgram(mShaderProgram);
        updateFaceCulling(mFaceCulling);
        updateDepthProperties(mZTest, mDepthBufferWrite);
        updateBlending(mBlend, mBlendDFactorAlpha, mBlendDFactorRGB, mBlendSFactorAlpha, mBlendSFactorRGB);
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
            ShaderBuildException::logCurrentCompileException(shader, (ShaderErrorType) type, source);
        }
        return move(shader);
    }
    
    void Shader::setShaderSource(ShaderType type, string str){
        shaderSources[type] = str;
    }
    
    std::string Shader::getShaderSource(ShaderType type) const {
        return shaderSources.at(type);
    }

    std::string Shader::getPrecompiledShaderSource(ShaderType type) const{
        string value = getShaderSource(type);
        return getPrecompiledSource(value, type);
    }

    const std::vector<AttributeDescriptor >& Shader::getShaderAttributes() const  {
        return mShaderAttributes;
    }
    
    const std::vector<UniformDescriptor >& Shader::getShaderUniforms() const {
        return shaderUniforms;
    }
    
    const AttributeDescriptor* Shader::getShaderAttribute(VertexAttributeSemantic semantic) const{
        for (int i=0;i< mShaderAttributes.size();i++){
            if (mShaderAttributes[i].semantic == semantic){
                return &mShaderAttributes[i];
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
            const int buffer_size = 512;
            array<char, buffer_size> lineBuffer;
            istringstream inbuf(source);
            string res;
            for (int i=1;i<=line+extraLines && inbuf.getline(lineBuffer.data(),buffer_size);i++){
                if (strncmp(lineBuffer.data(), "#line ", 6)==0){
                    string s{lineBuffer.data()};
                    static regex regExpSearchLine { R"((\d+))"};
                    auto iterLine = std::sregex_token_iterator(s.begin(), s.end(), regExpSearchLine, 1);
                    if (iterLine != endIter){
                        string newLineNumberStr = *iterLine;
                        i = atoi(newLineNumberStr.c_str());
                    }
                }
                if (i >= line-extraLines){
                    res += std::to_string(i)+"\t"+string{lineBuffer.data()}+"\n";
                }
            }
            return res;
        } else
            return "";
    }
    
    
    void ShaderBuildException::logCurrentCompileException(GLuint shader, ShaderErrorType type, string source){
        GLint logSize = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
        
        vector<char> errorLog((unsigned long) logSize);
        glGetShaderInfoLog(shader, logSize, &logSize, errorLog.data());
        string errorLines = extractLines(errorLog.data(), source);
        string typeStr;
        switch (type){
            case ShaderErrorType::FragmentShader:
                typeStr = "Fragment shader";
                break;
#ifndef GL_ES_VERSION_2_0
            case ShaderErrorType::GeometryShader:
                typeStr = "Geometry shader";
                break;
#endif
            case ShaderErrorType::IncompleteShader:
                typeStr = "Incomplete shader";
                break;
            case ShaderErrorType::Linker:
                typeStr = "Linker";
                break;
            case ShaderErrorType::VertexShader :
                typeStr = "Vertex shader";
                break;

        }
        logError(string{errorLog.data()}+"\n"+ typeStr +" error\n"+ errorLines);
    }
    
    void Shader::setBlend(bool b){ mBlend = b; }
    bool Shader::blend() { return mBlend; }
    BlendType Shader::blendDFactorAlpha() { return mBlendDFactorAlpha; }
    BlendType Shader::blendDFactorRGB() { return mBlendDFactorRGB; }
    BlendType Shader::blendSFactorAlpha() { return mBlendSFactorAlpha; }
    BlendType Shader::blendSFactorRGB() { return mBlendSFactorRGB; }
    void Shader::setBlendDFactorAlpha(BlendType blendDFactorAlpha) { this->mBlendDFactorAlpha =blendDFactorAlpha; }
    void Shader::setBlendDFactorRGB(BlendType blendDFactorRGB) { this->mBlendDFactorRGB =blendDFactorRGB; }
    void Shader::setBlendSFactorAlpha(BlendType blendSFactorAlpha) { this->mBlendSFactorAlpha = blendSFactorAlpha; }
    void Shader::setBlendSFactorRGB(BlendType blendSFactorRGB) { this->mBlendSFactorRGB = blendSFactorRGB; }
    void Shader::setDepthWrite(bool depthMask) { this->mDepthBufferWrite = depthMask;}
    bool Shader::depthWrite() { return mDepthBufferWrite; }
    void Shader::setFaceCulling(FaceCullingType faceCulling) { this->mFaceCulling = faceCulling; }
    FaceCullingType Shader::faceCulling() { return mFaceCulling; }
    void Shader::setPolygonOffsetEnabled(bool polygonOffsetEnabled) { this->mPolygonOffsetEnabled = polygonOffsetEnabled; }
    bool Shader::polygonOffsetEnabled() { return mPolygonOffsetEnabled; }
    void Shader::setPolygonOffsetFactorAndUnit(glm::vec2 polygonOffsetFactorAndUnit) { this->mPolygonOffsetFactorAndUnit = polygonOffsetFactorAndUnit; }
    glm::vec2 Shader::polygonOffsetFactorAndUnit() { return mPolygonOffsetFactorAndUnit; }
    void Shader::setZTest(ZTestType zTest) { this->mZTest = zTest; }
    ZTestType Shader::zTest() { return mZTest; }

    void Shader::bind_uniforms(Material *material, EngineUniforms *engineUniforms, Transform* transform){
        material->bind();
        SceneLights * sceneLights = engineUniforms->sceneLights;
        for (auto& uniform : shaderUniforms){
            if (uniform.name == UniformNames::modelMatrix){
                auto globalMatrix = transform->globalMatrix();
                glUniformMatrix4fv(uniform.index, 1, GL_FALSE, glm::value_ptr(globalMatrix));
            } else if (uniform.name == UniformNames::mv){
                auto modelView = engineUniforms->viewMatrix * transform->globalMatrix();
                glUniformMatrix4fv(uniform.index, 1, GL_FALSE, glm::value_ptr(modelView));
            } else if (uniform.name == UniformNames::norm) {
                auto modelView = engineUniforms->viewMatrix * transform->globalMatrix();
                auto normal = glm::inverseTranspose(glm::mat3(modelView));
                glUniformMatrix3fv(uniform.index, 1, GL_FALSE, glm::value_ptr(normal));
            } else if (uniform.name == UniformNames::v) {
                auto viewMatrix = engineUniforms->viewMatrix;
                glUniformMatrix4fv(uniform.index, 1, GL_FALSE, glm::value_ptr(viewMatrix));
            } else if (uniform.name == UniformNames::proj){
                glUniformMatrix4fv(uniform.index, 1, GL_FALSE, glm::value_ptr(engineUniforms->projectionMatrix));
            } else if (uniform.name == UniformNames::worldCamPos){
                auto cameraPos = engineUniforms->currentCameraTransform->position();
                glUniform3fv(uniform.index, 1, glm::value_ptr(cameraPos));
            } else if (uniform.name == UniformNames::world2object){
                auto world2object = transform->globalTRSInverse();
                glUniformMatrix4fv(uniform.index, 1, GL_FALSE, glm::value_ptr(world2object));
            } else if (uniform.name == UniformNames::mvProj){
                auto mvProj = engineUniforms->viewProjectionMatrix * transform->globalMatrix();
                glUniformMatrix4fv(uniform.index, 1, GL_FALSE, glm::value_ptr(mvProj));
            } else if (uniform.name == UniformNames::gameObjectUID){
                int32_t uid = transform->gameObject()->uniqueId();
                glm::vec4 packedInt = uint32ToVec4(uid);
                glUniform4fv(uniform.index, 1, glm::value_ptr(packedInt ));
            } else if (uniform.name == UniformNames::shadowMapTexture) {
                //throw invalid_argument("shadowMapTexture not yet implemented"); // todo
                cout <<"shadowMapTexture not yet implemented\n"; // todo
                logWarning("\"shadowMapTexture not yet implemented");
            } else if (uniform.name == UniformNames::lightMat){
                auto lightMatrix = engineUniforms->lightMatrix * transform->globalMatrix();
                glUniformMatrix4fv(uniform.index, 1, GL_FALSE, glm::value_ptr(lightMatrix));
            } else if (uniform.name == UniformNames::ambient){
                glm::vec3 ambientLight = sceneLights->ambientLight ? sceneLights->ambientLight->colorIntensity() : glm::vec3{0};
                glUniform3fv(uniform.index, 1, glm::value_ptr(ambientLight));
            } else if (uniform.name == UniformNames::pointLight){
                glUniformMatrix3fv(uniform.index, KICK_MAX_POINT_LIGHTS, GL_FALSE, glm::value_ptr(sceneLights->pointLightData[0]));
            } else if (uniform.name == UniformNames::directionalLight){
                glUniformMatrix3fv(uniform.index, 1, GL_FALSE, glm::value_ptr(sceneLights->directionalLightData));
            } else if (uniform.name == UniformNames::directionalLightWorld){
                glUniform3fv(uniform.index, 1, glm::value_ptr(sceneLights->directionalLightWorld));
            } else if (uniform.name == UniformNames::time){
                float time = Time::total();
                glUniform1f(uniform.index, time);
            } else if (uniform.name == UniformNames::viewport){
                glm::vec2 viewportSize = (glm::vec2)engineUniforms->viewportDimension.getValue();
                glUniform2fv(uniform.index, 1, glm::value_ptr(viewportSize));
            }

            // debug output
            //else {
            //    continue;
            //}
            //cout << to_string(uniform)<<"\n";
        }
    }

    void Shader::updateDefaultShaderLocation(){
        for (auto & e : defaultUniformData){
            updateShaderLocation(e.first, e.second);
        }
    }

    void Shader::updateShaderLocation(std::string name, MaterialData& value){
        auto descriptor = getShaderUniform(name);
        value.defaultUniform = true;
        if (descriptor == nullptr){
            value.shaderLocation = -1;
        } else {
            value.shaderLocation = descriptor->index;
        }
    }

    void Shader::setDefaultUniformData(std::string name, MaterialData &&value) {
        auto pos = defaultUniformData.find(name);
        if (pos != defaultUniformData.end()){
            pos->second.setValue(value.value);
            if (pos->second.glType != value.glType){
                pos->second.glType = value.glType;
                updateShaderLocation(name, pos->second);
            }
        } else {
            // not found insert new
            auto insertedElement = defaultUniformData.emplace(make_pair(name, move(value)));
            bool didInsert = insertedElement.second;
            assert(didInsert);
            auto insertedIterator = insertedElement.first;
            updateShaderLocation(name, insertedIterator->second);
        }
        shaderChanged.notifyListeners({this, ShaderEventType::defaultUniform });
    }

    bool Shader::tryGetDefaultUniform(std::string name, MaterialData &value) {
        auto pos = defaultUniformData.find(name);
        if (pos != defaultUniformData.end()){
            value = pos->second;
            return true;
        }
        return false;
    }

    int Shader::getRenderOrder() const {
        return mDenderOrder;
    }

    void Shader::setRenderOrder(int renderOrder) {
        if (renderOrder != Shader::mDenderOrder){
            Shader::mDenderOrder = renderOrder;
            shaderChanged.notifyListeners({this, ShaderEventType::shader });
        }
    }

    void Shader::setDefaultUniform(std::string name, int value) { setDefaultUniformInternal(name, value); }

    void Shader::setDefaultUniform(std::string name, float value) { setDefaultUniformInternal(name, value); }

    void Shader::setDefaultUniform(std::string name, glm::vec4 value) { setDefaultUniformInternal(name, value); }

    void Shader::setDefaultUniform(std::string name, glm::mat3 value) { setDefaultUniformInternal(name, value); }

    void Shader::setDefaultUniform(std::string name, glm::mat4 value) { setDefaultUniformInternal(name, value); }

    void Shader::setDefaultUniform(std::string name, std::shared_ptr<Texture2D> value) {
        texture2DRef[name] = value;
        setDefaultUniformInternal(name, value.get());
    }

    void Shader::setDefaultUniform(std::string name, std::shared_ptr<TextureCube> value) {
        textureCubeRef[name] = value;
        setDefaultUniformInternal(name, value.get());
    }
}