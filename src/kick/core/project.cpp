//
//  project.cpp
//  KickCPP
//
//  Created by morten on 8/25/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/core/project.h"
#include "kick/core/engine.h"
#include <fstream>
#include <SDL2_image/SDL_image.h>
#include "kick/texture/image_format.h"
#include "rapidjson/document.h"
#define GLM_FORCE_RADIANS
#include <glm/gtc/type_ptr.hpp>

using namespace std;

namespace kick {
    Project::Project()
    :assetLoader{new AssetLoader()}
    {
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


        int flags=IMG_INIT_JPG|IMG_INIT_PNG;
        int initted=IMG_Init(flags);
        if((initted&flags) != flags) {
            printf("IMG_Init: Failed to init required jpg and png support!\n");
            printf("IMG_Init: %s\n", IMG_GetError());
            // handle error
        }
    }
    
    Project *Project::getProjectRef(){
        Engine* engine = Engine::instance;
        return &(engine->project);
    }
    
    string Project::getResourceURI(string uri){
        return uri;
    }
    
    string Project::loadTextResource(string uri){
        ifstream file(getResourceURI(uri));
        if(!file.is_open())
            throw runtime_error(string{"couldn't open file "}+uri);
        
        string str;
        string res = "";
        while (std::getline(file, str))
        {
            if (res.length()>0){
                res = res + "\n";
            }
            res = res + str;
        }
        return res;
    }
    
    vector<char> Project::loadBinaryResource(string uri){
        ifstream file(uri, ios::in | ios::binary | ios::ate);
        if(!file.is_open())
            throw runtime_error(string{"couldn't open "}+uri);
        vector<char> fileContents((unsigned long) file.tellg());

        
        file.seekg(0, ios::beg);
        if(!file.read(&fileContents[ 0 ], fileContents.size()))
            throw runtime_error(string{"failed to read from "}+uri);
        return move(fileContents);
    }
    
    Texture2D* Project::loadTexture2D(std::string uri){
        vector<char> res = loadBinaryResource(uri);
        if (res.size()>0){
            return loadTexture2DFromMemory(&(res[0]), res.size());
        } else {
            return nullptr;
        }
    }
    
    void convertTextureIfNeeded(ImageFormat& imageFormat, SDL_Surface **image){
        SDL_Surface* imgRef = *image;
        SDL_PixelFormat * format = imgRef->format;
        auto pixelFormat = format->format;
        bool isBGR = format->Rshift == 16;
        GLenum RGB  = isBGR ? GL_BGR : GL_RGB;
        GLenum RGBA = isBGR ? GL_BGRA : GL_RGBA;
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
            
            ImageFormat imageFormat;
            if (image) {
                texturePtr = createAsset<Texture2D>();
                convertTextureIfNeeded(imageFormat, &image);
                invert_image(image->pitch, image->h, image->pixels);
                texturePtr->setData(image->w, image->h, static_cast<char*>(image->pixels), imageFormat);
                SDL_FreeSurface(image);
            } else {
                SDL_FreeRW(source);
            }
        }
        return texturePtr;
    }

    TextureCube* Project::loadTextureCubeFromMemory(const char *data, int size){
        TextureCube *texturePtr = nullptr;
        SDL_RWops* source = SDL_RWFromConstMem(data, size);
        if (source){
            // load uri into memory
            SDL_Surface *image = nullptr;
            image = IMG_Load_RW(source, 1);

            ImageFormat imageFormat;

            if (image) {
                texturePtr = createAsset<TextureCube>();
                convertTextureIfNeeded(imageFormat, &image);
                assert(image->h == image->w * 6);
                for (int i = 0; i < 6; i++) {
                    texturePtr->setData(image->w, image->h, static_cast<char *>(image->pixels) + image->pitch * image->w * i, i, imageFormat);
                }
                SDL_FreeSurface(image);
            } else {
                SDL_FreeRW(source);
            }

        }
        return texturePtr;
    }

    TextureCube* Project::loadTextureCube(std::string uri){
        vector<char> res = loadBinaryResource(uri);
        if (res.size()>0){
            return loadTextureCubeFromMemory(&(res[0]), res.size());
        } else {
            return nullptr;
        }
    }

    Shader* Project::loadShader(std::string uri){
        using namespace rapidjson;
        using namespace glm;
        string shaderSource = loadTextResource(uri);
        Document document;
        Shader* shader = nullptr;
        if (document.Parse<0>( shaderSource.c_str() ).HasParseError() ) {
            std::cout << "Error parsing" << std::endl;
        } else {
            shader = createAsset<Shader>();
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
            string vertexShader         = loadTextResource(vertexShaderURI);
            string fragmentShader       = loadTextResource(fragmentShaderURI);
            
            shader->setShaderSource(ShaderType::VertexShader, vertexShader);
            shader->setShaderSource(ShaderType::FragmentShader, fragmentShader);
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
                                Texture2D* value = loadTexture2D(texName);
                                shader->setDefaultUniform(name, value);
                            }
                                break;
                            case GL_SAMPLER_CUBE:
                            {
                                string texName = memberIter->value.GetString();
                                TextureCube* value = loadTextureCube(texName);
                                shader->setDefaultUniform(name, value);
                            }
                                break;
                        }
                    }
                }
            }
        }
        return shader;
    }

    
    int Project::mapAssetURIToId(std::string assetURI){
        // todo implement
        // store assetId in file? Or database? (currently the id assignment can change on program execution)
        static int tempAssetCounter = 0;
        auto valueIter = assetIdMap.find(assetURI);
        if (valueIter != assetIdMap.end()){
            return valueIter->second;
        }
        tempAssetCounter--;
        assetIdMap[assetURI] = tempAssetCounter;
        return tempAssetCounter;
    }
}
