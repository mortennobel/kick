//
// Created by morten on 30/06/14.
//


#include "kick/core/project_asset.h"
#include "kick/core/project.h"
#include "glm/glm.hpp"

#pragma once

namespace kick {
    class Texture2D;

    struct TextureAtlasEntry {
        bool rotated = false;
        bool trimmed = false;
        glm::ivec4 frame = glm::ivec4{0};
        glm::ivec4 spriteSourceSize = glm::ivec4{0};
        glm::ivec2 sourceSize = glm::ivec2{0};
    };

    class TextureAtlas : public ProjectAsset  {
    public:
        TextureAtlas(Project *project);

        bool load(std::string filename, std::string texture);

        Texture2D * getTexture() const;

        TextureAtlasEntry get(std::string name);

        glm::ivec2 getTextureSize() const;
    private:
        glm::ivec2 textureSize;
        std::map<std::string, TextureAtlasEntry> atlas;
        Texture2D* texture = nullptr;
    };
}
