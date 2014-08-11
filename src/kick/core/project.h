//
//  project.h
//  KickCPP
//
//  Created by morten on 8/25/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once

#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>
#ifdef EMSCRIPTEN
#   include <SDL/SDL_image.h>
#else
#   include <SDL2_image/SDL_image.h>
#endif
#include "kick/core/project_asset.h"
#include "kick/core/asset_loader.h"
#include "kick/texture/texture2d.h"
#include "kick/material/shader.h"
#include "kick/texture/texture_atlas.h"


namespace kick {
    class Engine;
    class TextureAtlas;
    class Font;

    class Project {
    public:
        template <class E,  typename ... Args>
        static E* createAsset(Args ... args);

        static void destroyAsset(ProjectAsset* asset);
        /*
        template <class E>
        E* getAsset(int assetId){
            ProjectAsset *ptr;
            if (assetId < 0){
                return loadAsset<E>(assetId);
            } else {
                ptr = assets[assetId].get();
            }
            return dynamic_cast<E*>(ptr);
        }
        
        template <class E>
        E* getAsset(std::string assetURI){
            int assetId = mapAssetURIToId(assetURI);
            return getAsset<E>(assetId);
        }
        
        
        
        template <class E>
        E* loadAsset(int id){
            E* ptr = dynamic_cast<E*>(assets[id]);
            if (!ptr) {
                ptr = assetLoader->loadAsset(id);
                if (ptr) {
                    assets[id] = std::unique_ptr<E>(ptr);
                }
            }
            return ptr;
        }*/
        

        static bool loadTextResource(std::string uri, std::string &outText);
        static bool loadBinaryResource(std::string uri, std::vector<char> &outResource);
        static std::shared_ptr<Shader> loadShader(std::string uri);
        static std::shared_ptr<Texture2D> loadTexture2D(std::string uri);
        static std::shared_ptr<Font> loadFont(std::string fontName = "assets/font/open_sans_28.fnt");
        static Texture2D* loadTexture2DFromMemory(const char *data, int size);

        static std::shared_ptr<TextureCube> loadTextureCube(std::string uri);
        static TextureCube* loadTextureCubeFromMemory(const char *data, int size);

        // filename should be the .txt file. There should be a png file with the same name in the same folder containing
        // the actual sprites
        static std::shared_ptr<TextureAtlas> loadTextureAtlas(std::string filename);

        friend class ProjectAsset;
        friend class Engine;
    private:
        int maxAssetId = 1;
        Project();
        Project(const Project&) = delete;
        Project(Project&&) = delete;
        static Project * getProjectRef();
        std::unordered_map<int, std::unique_ptr<ProjectAsset>> assets;
        std::unique_ptr<AssetLoader> assetLoader;
        int mapAssetURIToId(std::string assetURI);
        std::unordered_map<std::string, int> assetIdMap;

        static Texture2D *surfaceToTexture2D( SDL_Surface *image);

        static TextureCube *surfaceToTextureCube(SDL_Surface *image);

        static std::map<std::string, std::weak_ptr<TextureAtlas>> textureAtlasRef;
        static std::map<std::string, std::weak_ptr<Shader>> shaderRef;
        static std::map<std::string, std::weak_ptr<Texture2D>> texture2DRef;
        static std::map<std::string, std::weak_ptr<TextureCube>> textureCubeRef;
        static std::map<std::string, std::weak_ptr<Font>> fontRef;
    };

    template <class E,  typename ... Args>
    inline E* Project::createAsset(Args ... args){
        E* res = new E{getProjectRef(), args...};
        //res->uid = maxAssetId;
        //assets[maxAssetId] = std::unique_ptr<E>(res);
        //maxAssetId++;
        return res;
    }

}
