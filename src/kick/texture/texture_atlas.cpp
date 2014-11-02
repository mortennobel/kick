//
// Created by morten on 30/06/14.
//


#include "kick/texture/texture_atlas.h"
#include <rapidjson/document.h>
#include "kick/core/project.h"
#include "kick/material/shader.h"



using namespace std;
using namespace glm;

namespace kick {

    namespace {
        ivec2 whToVec2(rapidjson::Value &val) {
            return {val["w"].GetInt(), val["h"].GetInt()};
        }

        ivec4 xywhToVec4(rapidjson::Value &val) {
            return {val["x"].GetInt(), val["y"].GetInt(), val["w"].GetInt(), val["h"].GetInt()};
        }
    }

    TextureAtlas::TextureAtlas()
    {
        mShader = Project::loadShader("assets/shaders/sprite.shader");
    }

    bool TextureAtlas::load(std::string filename, std::string texture) {
        this->mTexture = Project::loadTexture2D(texture);
        string textureAtlas;
        if (!Project::loadTextResource(filename, textureAtlas)){
            return false;
        }

        mAtlas.clear();

        rapidjson::Document d;
        d.Parse<0>(textureAtlas.c_str());
        auto &frames = d["frames"];
        for (auto itr = frames.MemberBegin(); itr != frames.MemberEnd(); ++itr){
            TextureAtlasEntry entry;
            auto& val = itr->value;
            entry.rotated = val["rotated"].GetBool();
            entry.trimmed = val["trimmed"].GetBool();
            entry.frame = xywhToVec4(val["frame"]);
            entry.spriteSourceSize = xywhToVec4(val["spriteSourceSize"]);
            entry.sourceSize = whToVec2(val["sourceSize"]);
            mAtlas[itr->name.GetString()] = entry;
        }
        auto &meta = d["meta"];
        mTextureSize = whToVec2(meta["size"]);
        return true;
    }

    shared_ptr<Texture2D> TextureAtlas::texture() const {
        return mTexture;
    }

    TextureAtlasEntry TextureAtlas::get(std::string name) {
        return mAtlas[name];
    }

    glm::ivec2 TextureAtlas::textureSize() const {
        return mTextureSize;
    }

    std::shared_ptr<Shader> TextureAtlas::shader() const {
        return mShader;
    }

    void TextureAtlas::setShader(std::shared_ptr<Shader> shader) {
        TextureAtlas::mShader = shader;
    }
}