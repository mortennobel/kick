//
// Created by morten on 30/06/14.
//


#pragma once
#include <string>
#include "kick/core/project_asset.h"
#include "kick/core/project.h"


namespace kick {
    class Texture2D;
    class Material;
    class Shader;
    class Text;

    struct FontChar {
//        int id;
        int x;
        int y;
        int width;
        int height;
        int xoffset;
        int yoffset;
        int xadvance;
        int page;
        int chnl;
//        std::string letter;
    };

class Font : public ProjectAsset {
public:
    Font(Project *project);
    ~Font();
    void loadFntFile(std::string filename, std::string texturename);
    int width(std::string text);
    int height();


    Texture2D * getTexture() const;

    void setTexture(Texture2D *texture);

    const FontChar getChar(char r);

    int getScaleW() const;
    int getScaleH() const;

    Shader * getShader();

    void setShader(Shader* shader);

    int getBase() const;

    friend class Text;
private:
    Texture2D* texture;
    int lineHeight = 0;
    int base = 0;
    int scaleW = 0;
    int scaleH = 0;
    int pages = 1;
    std::map<int, FontChar> fontMap;
    Material *material = nullptr;
};
}
