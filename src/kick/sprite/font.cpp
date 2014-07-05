//
// Created by morten on 30/06/14.
//

#include "font.h"
#include "project.h"
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

namespace kick{
    Font::Font(Project *project) : ProjectAsset(project) {
        material = project->createAsset<Material>();
        material->setShader(project->loadShader("assets/shaders/transparent_unlit.shader"));
    }

    Font::~Font() {
        project->destroyAsset(material);
    }

    std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
        elems.clear();
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
    }

    void Font::loadFntFile(std::string filename, std::string texturename) {
        fontMap.clear();
        setTexture(Project::loadTexture2D(texturename));
        string fntFile = Project::loadTextResource(filename);
        std::istringstream f{fntFile};
        std::string line;
        std::vector<string> elems;
        std::vector<string> keyval;
        while (std::getline(f, line)) {
            split(line, ' ', elems);
            if (elems.empty())
                continue;
            if (elems[0] == "info") {
            } else if (elems[0] == "common") {
                for (int i=1;i<elems.size();i++) {
                    split(elems[i], '=', keyval);
                    if (keyval[0] == "lineHeight"){
                        lineHeight = stoi(keyval[1]);
                    } else if (keyval[0] == "base"){
                        base = stoi(keyval[1]);
                    } else if (keyval[0] == "scaleW"){
                        scaleW = stoi(keyval[1]);
                    } else if (keyval[0] == "scaleH"){
                        scaleH = stoi(keyval[1]);
                    } else if (keyval[0] == "pages"){
                        pages = stoi(keyval[1]);
                    }
                }
            } else if (elems[0] == "page") {
            } else if (elems[0] == "chars") {
            } else if (elems[0] == "char") {
                FontChar f{};
                int id = -1;
                for (int i=1;i<elems.size();i++){
                    split(elems[i], '=', keyval);
                    if (keyval[0] == "id"){
                        id = stoi(keyval[1]);
                    } else if (keyval[0] == "x"){
                        f.x = stoi(keyval[1]);
                    } else if (keyval[0] == "y"){
                        f.y = stoi(keyval[1]);
                    } else if (keyval[0] == "width"){
                        f.width = stoi(keyval[1]);
                    } else if (keyval[0] == "height"){
                        f.height = stoi(keyval[1]);
                    } else if (keyval[0] == "xoffset"){
                        f.xoffset = stoi(keyval[1]);
                    } else if (keyval[0] == "yoffset"){
                        f.yoffset = stoi(keyval[1]);
                    } else if (keyval[0] == "xadvance"){
                        f.xadvance = stoi(keyval[1]);
                    } else if (keyval[0] == "page"){
                        f.page = stoi(keyval[1]);
                    } else if (keyval[0] == "chnl"){
                        f.chnl = stoi(keyval[1]);
                    } else if (keyval[0] == "letter"){
                        //    f.letter = keyval[1].substr (1, (unsigned long) (keyval[1].length()-2));
                    }
                }
                fontMap[id] = f;
            } else if (elems[0] == "kernings") {

            } else {
                cout << "Not found type "<<elems[0]<<endl;
            }
            elems.clear();
        }
    }

    int Font::width(std::string text) {
        int sum = 0;
        for (auto f : text){
            auto found = fontMap.find((int)f);
            if (found != fontMap.end()){
                sum += found->second.width;
            } else {
                return -1;
            }
        }
        return sum;
    }

    int Font::height() {
        return lineHeight;
    }

    Texture2D *Font::getTexture() const {
        return texture;
    }

    void Font::setTexture(Texture2D *texture) {
        material->setUniform("mainTexture", texture);
        Font::texture = texture;
    }

    const FontChar Font::getChar(char r) {
        auto found = fontMap.find((int)r);
        if (found != fontMap.end()){
            return found->second;
        } else {
            return kick::FontChar{-1,-1,-1,-1};
        }
    }

    int Font::getScaleW() const {
        return scaleW;
    }

    int Font::getScaleH() const {
        return scaleH;
    }

    int Font::getBase() const {
        return base;
    }

    void Font::setShader(Shader* shader) {
        material->setShader(shader);
    }

    Shader *Font::getShader() {
        return material->getShader();
    }
}