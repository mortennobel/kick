//
//  project.cpp
//  KickCPP
//
//  Created by morten on 8/25/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/core/project.h"
#include "kick/core/engine.h"
#include "kick/core/log.h"
#include <fstream>

#include "kick/texture/image_format.h"
#include "rapidjson/document.h"

#include <glm/gtc/type_ptr.hpp>
#include "kick/texture/texture_atlas.h"
#include "kick/2d/font.h"
#ifdef EMSCRIPTEN
#   include <SDL/SDL_image.h>
#else
#   ifdef _WIN32
#       include <SDL_image.h>
#   else
#       include <SDL2_image/SDL_image.h>
#   endif
#endif
using namespace std;

namespace kick {

    std::map<std::string, std::weak_ptr<TextureAtlas>> Project::textureAtlasRef;
    std::map<std::string, std::weak_ptr<Shader>> Project::shaderRef;
    std::map<std::string, std::weak_ptr<Texture2D>> Project::texture2DRef;
    std::map<std::string, std::weak_ptr<TextureCube>> Project::textureCubeRef;
    std::map<std::string, std::weak_ptr<Font>> Project::fontRef;

    Project::Project()
    {
#ifndef EMSCRIPTEN
        SDL_version compile_version;
        const SDL_version *link_version=IMG_Linked_Version();
        SDL_IMAGE_VERSION(&compile_version);
        if (compile_version.major != link_version->major ||
                compile_version.minor != link_version->minor ||
                compile_version.patch != link_version->patch){
            printf("compiled with SDL_image version: %d.%d.%d\n",
                    compile_version.major,
                    compile_version.minor,
                    compile_version.patch);
            printf("running with SDL_image version: %d.%d.%d\n",
                    link_version->major,
                    link_version->minor,
                    link_version->patch);
        } else {
            printf("SDL_image version: %d.%d.%d\n",
                    compile_version.major,
                    compile_version.minor,
                    compile_version.patch);
        }
#endif

        int flags=IMG_INIT_JPG|IMG_INIT_PNG;
        int initted=IMG_Init(flags);
        if((initted&flags) != flags) {
            printf("IMG_Init: Failed to init required jpg and png support!\n");
            printf("IMG_Init: %s\n", IMG_GetError());
            // handle error
        }
    }
    



    bool Project::loadTextResource(std::string uri, std::string &res){
        ifstream file(uri);
        if(!file.is_open()) {
            logError(string{"couldn't open file "} + uri);
            return false;
        }

        string str;
        res = "";
        while (std::getline(file, str))
        {
            if (res.length()>0){
                res = res + "\n";
            }
            res = res + str;
        }
        return true;
    }
    
    bool Project::loadBinaryResource(string uri, vector<char>& fileContents){
        ifstream file(uri, ios::in | ios::binary | ios::ate);
        if(!file.is_open()){
            logError(string{"couldn't open "}+uri);
            return false;
        }
        fileContents.resize((unsigned long) file.tellg());

        file.seekg(0, ios::beg);
        if(!file.read(&fileContents[ 0 ], fileContents.size())){
            logError(string{"failed to read from "}+uri);
            return false;
        }
        return true;
    }
    
    shared_ptr<Texture2D> Project::loadTexture2D(std::string uri){
        SDL_Surface * surface = IMG_Load(uri.c_str());
        if (surface){
            return shared_ptr<Texture2D>{surfaceToTexture2D(surface)};
        } else {
            return shared_ptr<Texture2D>{};
        }
    }
    
    void convertTextureIfNeeded(ImageFormat& imageFormat, SDL_Surface **image){
        SDL_Surface* imgRef = *image;
        SDL_PixelFormat * format = imgRef->format;
        auto pixelFormat = format->format;
        bool isBGR = format->Rshift == 16;
#ifdef GL_ES_VERSION_2_0
        GLenum RGB  = GL_RGB;
        GLenum RGBA = GL_RGBA;
#else
        GLenum RGB  = isBGR ? GL_BGR : GL_RGB;
        GLenum RGBA = isBGR ? GL_BGRA : GL_RGBA;

#endif
        const bool alpha = SDL_ISPIXELFORMAT_ALPHA(pixelFormat);
        if (alpha){
            imageFormat.format = RGBA;
            imageFormat.internalFormat = GL_RGBA;
            imageFormat.type = GL_UNSIGNED_BYTE;
        } else {
            if (format->BytesPerPixel == 4){
                imageFormat.format = RGBA;
                imageFormat.internalFormat = GL_RGBA;
            } else {
                imageFormat.format = RGB;
                imageFormat.internalFormat = GL_RGB;
            }
            imageFormat.type = GL_UNSIGNED_BYTE;
        }
    }
    
    int invert_image(int width, int height, void* image_pixels)
    {
        auto temp_row = unique_ptr<char>(new char[width]);
        if (temp_row.get() == nullptr)
        {
            SDL_SetError("Not enough memory for image inversion");
            return -1;
        }
        //if height is odd, don't need to swap middle row
        int height_div_2 = (int) (height * .5f);
        for(int index = 0; index < height_div_2; index++)    {
            //uses string.h
            memcpy((Uint8 *)temp_row.get(),
                   (Uint8 *)(image_pixels) +
                   width * index,
                    width);
            
            memcpy(
                   (Uint8 *)(image_pixels) +
                   width * index,
                   (Uint8 *)(image_pixels) +
                   width * (height - index-1),
                    width);
            memcpy(
                   (Uint8 *)(image_pixels) +
                   width * (height - index-1),
                   temp_row.get(),
                    width);
        }
        return 0;
    }
    
    Texture2D* Project::loadTexture2DFromMemory(const char *data, int size){
        Texture2D *texturePtr = nullptr;
        SDL_RWops* source = SDL_RWFromConstMem(data, size);
        if (source){
            // load uri into memory
            SDL_Surface *image = nullptr;
            image = IMG_Load_RW(source, 1);
            
            if (image) {
                texturePtr = surfaceToTexture2D(image);
            } else {
                SDL_FreeRW(source);
            }
        }
        return texturePtr;
    }

    Texture2D *Project::surfaceToTexture2D(SDL_Surface *image) {
        ImageFormat imageFormat;
        convertTextureIfNeeded(imageFormat, &image);
#if EMSCRIPTEN
        const GLenum GL_UNPACK_FLIP_Y_WEBGL = 37440;
        glPixelStorei(GL_UNPACK_FLIP_Y_WEBGL, true);
#else
        invert_image(image->pitch, image->h, image->pixels);
#endif
        Texture2D *texturePtr = new Texture2D();
        texturePtr->setData(image->w, image->h, static_cast<char*>(image->pixels), imageFormat);
#if EMSCRIPTEN
        glPixelStorei(GL_UNPACK_FLIP_Y_WEBGL, false);
#endif
        SDL_FreeSurface(image);
        return texturePtr;
    }

    TextureCube* Project::loadTextureCubeFromMemory(const char *data, int size){
        TextureCube *texturePtr = nullptr;
        SDL_RWops* source = SDL_RWFromConstMem(data, size);
        if (source){
            // load uri into memory
            SDL_Surface *image = nullptr;
            image = IMG_Load_RW(source, 1);

            if (image) {
                texturePtr = surfaceToTextureCube(image);
            } else {
                SDL_FreeRW(source);
            }
        }
        return texturePtr;
    }

    TextureCube *Project::surfaceToTextureCube( SDL_Surface *image) {
        ImageFormat imageFormat;
        TextureCube *texturePtr = new TextureCube();
        convertTextureIfNeeded(imageFormat, &image);
        assert(image->h == image->w * 6);
        for (int i = 0; i < 6; i++) {
            texturePtr->setData(image->w, image->h, static_cast<char *>(image->pixels) + image->pitch * image->w * i, i, imageFormat);
        }
        SDL_FreeSurface(image);
        return texturePtr;
    }

    shared_ptr<TextureCube> Project::loadTextureCube(std::string uri){
        auto iter = textureCubeRef.find(uri);
        if (iter != textureCubeRef.end()){
            if (!iter->second.expired()){
                return iter->second.lock();
            }
        }

        SDL_Surface * surface = IMG_Load(uri.c_str());
        if (surface){
            auto res = shared_ptr<TextureCube>{surfaceToTextureCube(surface)};
            textureCubeRef[uri] = std::weak_ptr<TextureCube>{res};
            return res;
        } else {
            return shared_ptr<TextureCube>{nullptr};
        }
    }

    Material* Project::createMaterial(std::string shaderUri){
        auto shader = loadShader(shaderUri);
        if (!shader){
            return nullptr;
        }
        return new Material(shader);
    }


    shared_ptr<Shader> Project::loadShader(std::string uri){
        auto iter = shaderRef.find(uri);
        if (iter != shaderRef.end()){
            if (!iter->second.expired()){
                return iter->second.lock();
            }
        }

        using namespace rapidjson;
        using namespace glm;
        string shaderSource;
        bool success = loadTextResource(uri, shaderSource);
        if (!success){
            return nullptr;
        }

        Document document;
        Shader* shader = nullptr;
        if (document.Parse<0>( shaderSource.c_str() ).HasParseError() ) {
            logError("Error parsing .shader file");
        } else {
            shader = new Shader();
            auto getString = [&](string name, string defaultValue){
                if (!document.HasMember(name.c_str())){
                    return defaultValue;
                }
                return string{document[name.c_str()].GetString()};
            };
            
            auto getBool = [&](string name, bool defaultValue){
                if (!document.HasMember(name.c_str())){
                    return defaultValue;
                }
                return document[name.c_str()].GetBool();
            };
            
            auto getFloat = [&](string name, float defaultValue){
                if (!document.HasMember(name.c_str())){
                    return defaultValue;
                }
                return (float)document[name.c_str()].GetDouble();
            };

            auto getInt = [&](string name, int defaultValue){
                if (!document.HasMember(name.c_str())){
                    return defaultValue;
                }
                return document[name.c_str()].GetInt();
            };
            
#define createGetX(X) auto get##X = [&](string name, X defaultValue){ \
                if (!document.HasMember(name.c_str())){ \
                    return defaultValue; \
                } \
                int val = (int)round(document[name.c_str()].GetDouble()); \
                X res {defaultValue}; \
                toEnum(val, res); \
                return res; \
            }
            createGetX(BlendType);
            createGetX(FaceCullingType);
            createGetX(ZTestType);
            
            string vertexShaderURI      = getString("vertexShaderURI", "");
            string geometryShaderURI      = getString("geometryShaderURI", "");
            string fragmentShaderURI    = getString("fragmentShaderURI", "");
            //string outputAttributeName  = getString("outputAttributeName", "");
            bool blend                  = getBool("blend", false);
            BlendType blendDFactorAlpha = getBlendType("blendDFactorAlpha", BlendType::OneMinusSrcAlpha);
            BlendType blendDFactorRGB   = getBlendType("blendDFactorRGB", BlendType::OneMinusSrcAlpha);
            BlendType blendSFactorAlpha = getBlendType("blendSFactorAlpha", BlendType::SrcAlpha);
            BlendType blendSFactorRGB   = getBlendType("blendSFactorRGB", BlendType::SrcAlpha);
            bool depthWrite             = getBool("depthWrite", true);
            FaceCullingType faceCulling = getFaceCullingType("faceCulling", FaceCullingType::Back);
            bool polygonOffsetEnabled   = getBool("polygonOffsetEnabled", false);
            float polygonOffsetFactor   = getFloat("polygonOffsetFactor", 1);
            float polygonOffsetUnit     = getFloat("polygonOffsetUnit", 0);
            ZTestType zTest             = getZTestType("zTest", ZTestType::Less);
            int renderOrder             = getInt("renderOrder", 1000);
            string vertexShader;
            if (!loadTextResource(vertexShaderURI, vertexShader)){
                return nullptr;
            }
            string fragmentShader;
            if (!loadTextResource(fragmentShaderURI,fragmentShader)){
                return nullptr;
            }


            shader->setShaderSource(ShaderType::VertexShader, vertexShader);
            shader->setShaderSource(ShaderType::FragmentShader, fragmentShader);

            if (geometryShaderURI.length()){
#ifdef GL_ES_VERSION_2_0
                assert(false);
#else
                string geometryShader;
                if (!loadTextResource(geometryShaderURI, geometryShader)){
                    return nullptr;
                }
                shader->setShaderSource(ShaderType::GeometryShader, geometryShader);
#endif
            }

            shader->setBlend(blend);
            shader->setBlendDFactorAlpha(blendDFactorAlpha);
            shader->setBlendDFactorRGB(blendDFactorRGB);
            shader->setBlendSFactorAlpha(blendSFactorAlpha);
            shader->setBlendSFactorRGB(blendSFactorRGB);
            shader->setDepthWrite(depthWrite);
            shader->setFaceCulling(faceCulling);
            shader->setPolygonOffsetEnabled(polygonOffsetEnabled);
            shader->setPolygonOffsetFactorAndUnit(vec2{polygonOffsetFactor, polygonOffsetUnit});
            shader->setZTest(zTest);
            shader->setRenderOrder(renderOrder);
            shader->apply();

            if (document.HasMember("defaultUniform") && document["defaultUniform"].IsObject()){
                auto & defaultUniformArray = document["defaultUniform"];
                for (auto memberIter = defaultUniformArray.MemberBegin();memberIter != defaultUniformArray.MemberEnd();memberIter++){
                    string name = memberIter->name.GetString();
                    auto& uniformValue = memberIter->value;
                    const UniformDescriptor * uniform = shader->getShaderUniform(name);
                    if (uniform) {
                        switch (uniform->type){
                            case GL_INT:
                            {
                                int value = uniformValue.GetInt();
                                shader->setDefaultUniform(name, value);
                            }
                                break;
                            case GL_FLOAT:
                            {
                                float value = (float) uniformValue.GetDouble();
                                shader->setDefaultUniform(name, value);
                            }
                                break;
                            case GL_FLOAT_VEC4:
                            {
                                vec4 value;
                                for (SizeType i=0;i<4;i++){
                                    value[i] = (float)uniformValue[i].GetDouble();
                                }
                                shader->setDefaultUniform(name, value);
                            }
                                break;
                            case GL_FLOAT_MAT3:
                            {
                                mat3 value;
                                float *valuePtr = value_ptr(value);
                                for (SizeType i=0;i<9;i++){
                                    valuePtr[i] = (float)uniformValue[i].GetDouble();
                                }
                                shader->setDefaultUniform(name, value);
                            }
                                break;
                            case GL_FLOAT_MAT4:
                            {
                                mat4 value;
                                float *valuePtr = value_ptr(value);
                                for (SizeType i = 0; i < 16; i++) {
                                    valuePtr[i] = (float) uniformValue[i].GetDouble();
                                }
                                shader->setDefaultUniform(name, value);
                            }
                                break;
                            case GL_SAMPLER_2D:
                            {
                                string texName = memberIter->value.GetString();
                                auto value = loadTexture2D(texName);
                                shader->setDefaultUniform(name, value);
                            }
                                break;
                            case GL_SAMPLER_CUBE:
                            {
                                string texName = memberIter->value.GetString();
                                auto value = loadTextureCube(texName);
                                shader->setDefaultUniform(name, value);
                            }
                                break;
                            default:{
                                cout << "Unhandled case " << uniform->type << endl;
                            }
                            break;
                        }
                    }
                }
            }
        }
        auto ref = std::shared_ptr<Shader>{shader};
        shaderRef[uri] = weak_ptr<Shader>{ref};
        return ref;
    }

    std::shared_ptr<TextureAtlas> Project::loadTextureAtlas(std::string filename) {
        std::string texture = filename.substr(0, filename.size()-4) + ".png";

        auto iter = textureAtlasRef.find(filename);
        if (iter != textureAtlasRef.end()){
            if (!iter->second.expired()){
                return iter->second.lock();
            }
        }
        TextureAtlas* textureAtlas = new TextureAtlas();
        textureAtlas->load(filename, texture);
        auto ref = std::shared_ptr<TextureAtlas>{textureAtlas};
        textureAtlasRef[filename] = weak_ptr<TextureAtlas>{ref};
        return ref;
    }

    std::shared_ptr<Font> Project::loadFont(string fontName) {
        auto iter = fontRef.find(fontName);
        if (iter != fontRef.end()){
            if (!iter->second.expired()){
                return iter->second.lock();
            }
        }
        Font* font = new Font();
        font->loadFntFile(fontName);
        auto ref = std::shared_ptr<Font>{font};
        fontRef[fontName] = weak_ptr<Font>{ref};
        return ref;
    }
}
