//
// Created by morten on 30/06/14.
//


#include "kick/core/project_asset.h"
#include "kick/core/project.h"
#include "glm/glm.hpp"

#pragma once

namespace kick {
    struct TextureAtlasEntry {
        bool rotated;
        bool trimmed;
        glm::ivec4 frame;
        glm::ivec4 spriteSourceSize;
        glm::ivec2 sourceSize;
    };

class TextureAtlas : public ProjectAsset  {
public:
    TextureAtlas(Project *project);

    void load(std::string filename);
private:
    glm::ivec2 textureSize;
    std::map<std::string, TextureAtlasEntry> atlas;
};
}


