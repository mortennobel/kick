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

#include "kick/core/project_asset.h"
#include "kick/texture/texture2d.h"
#include "kick/material/shader.h"
#include "kick/texture/texture_atlas.h"

// forward declaration
struct SDL_Surface;

namespace kick {
    class Engine;
    class TextureAtlas;
    class Font;

    class Project {
    public:
        // Create a new material with a specific shader attached
        // Returns nullptr if shader cannot be loaded
        static Material *createMaterial(std::string shaderUri = "assets/shaders/diffuse.shader");

        static bool loadTextResource(std::string uri, std::string &outText);
        static bool loadBinaryResource(std::string uri, std::vector<char> &outResource);
        static std::shared_ptr<Shader> loadShader(std::string uri);
        static std::shared_ptr<Texture2D> loadTexture2D(std::string uri, TextureSampler sampler = {});
        static std::shared_ptr<Font> loadFont(std::string fontName = "assets/font/open_sans_28.fnt");
        static std::shared_ptr<Texture2D> loadTexture2DFromMemory(const char *data, int size);

        static std::shared_ptr<TextureCube> loadTextureCube(std::string uri);
        static TextureCube* loadTextureCubeFromMemory(const char *data, int size);

        // filename should be the .txt file. There should be a png file with the same name in the same folder containing
        // the actual sprites
        static std::shared_ptr<TextureAtlas> loadTextureAtlas(std::string filename);

        friend class ProjectAsset;
        friend class Engine;
    private:
        Project();
        Project(const Project&) = delete;
        Project(Project&&) = delete;

        static Texture2D *surfaceToTexture2D( SDL_Surface *image, TextureSampler sampler = {});

        static TextureCube *surfaceToTextureCube(SDL_Surface *image);

        static std::map<std::string, std::weak_ptr<TextureAtlas>> textureAtlasRef;
        static std::map<std::string, std::weak_ptr<Shader>> shaderRef;
        static std::map<std::string, std::weak_ptr<Texture2D>> texture2DRef;
        static std::map<std::string, std::weak_ptr<TextureCube>> textureCubeRef;
        static std::map<std::string, std::weak_ptr<Font>> fontRef;


    };
}
