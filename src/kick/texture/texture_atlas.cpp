//
// Created by morten on 30/06/14.
//


#include "kick/texture/texture_atlas.h"
#include <rapidjson/document.h>



using namespace std;
using namespace glm;

namespace kick {

    TextureAtlas::TextureAtlas(Project *project) : ProjectAsset(project) {
    }

    ivec2 whToVec2(rapidjson::Value &val){
        return {val["w"].GetInt(), val["h"].GetInt()};
    }

    ivec4 xywhToVec4(rapidjson::Value &val){
        return {val["x"].GetInt(), val["y"].GetInt(),val["w"].GetInt(), val["h"].GetInt()};
    }

    void TextureAtlas::load(std::string filename) {
        string textureAtlas = Project::loadTextResource(filename);

        atlas.clear();

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
            atlas[itr->name.GetString()] = entry;
        }
        auto &meta = d["meta"];
        textureSize = whToVec2(meta["size"]);

    }
}