//
// Created by morten on 30/06/14.
//

#pragma once
#include "kick/core/project_asset.h"
#include "glm/glm.hpp"
#include <map>

namespace kick {
    class Texture2D;
    class TextureAtlas;
    class Project;
    class Shader;


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

        std::shared_ptr<Texture2D> getTexture() const;
        TextureAtlasEntry get(std::string name);
        glm::ivec2 getTextureSize() const;

        std::shared_ptr<Shader> getShader() const;
        void setShader(std::shared_ptr<Shader> shader);
    private:
        glm::ivec2 textureSize;
        std::map<std::string, TextureAtlasEntry> atlas;
        std::shared_ptr<Texture2D> texture;
        std::shared_ptr<Shader> shader;
    };
}
