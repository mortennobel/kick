//
// Created by morten on 30/06/14.
//

#include "kick/2d/font.h"
#include "kick/core/project.h"
#include "log.h"
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

namespace kick{
    Font::Font(Project *project) : ProjectAsset(project) {
        shader = project->loadShader("assets/shaders/font.shader");
    }

    Font::~Font() {
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

    bool Font::loadFntFile(string filename) {
        string texturename = filename.substr(0, filename.size()-4) + ".png";
        fontMap.clear();
        kerning.clear();
        setTexture(Project::loadTexture2D(texturename));
        if (!texture){
            return false;
        }
        string fntFile;
        if (!Project::loadTextResource(filename,fntFile)){
            return false;
        }
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
                FontChar fc{};
                int id = -1;
                for (int i=1;i<elems.size();i++){
                    split(elems[i], '=', keyval);
                    if (keyval[0] == "id"){
                        id = stoi(keyval[1]);
                    } else if (keyval[0] == "x"){
                        fc.x = stoi(keyval[1]);
                    } else if (keyval[0] == "y"){
                        fc.y = stoi(keyval[1]);
                    } else if (keyval[0] == "width"){
                        fc.width = stoi(keyval[1]);
                    } else if (keyval[0] == "height"){
                        fc.height = stoi(keyval[1]);
                    } else if (keyval[0] == "xoffset"){
                        fc.xoffset = stoi(keyval[1]);
                    } else if (keyval[0] == "yoffset"){
                        fc.yoffset = stoi(keyval[1]);
                    } else if (keyval[0] == "xadvance"){
                        fc.xadvance = stoi(keyval[1]);
                    } else if (keyval[0] == "page"){
                        fc.page = stoi(keyval[1]);
                    } else if (keyval[0] == "chnl"){
                        fc.chnl = stoi(keyval[1]);
                    } else if (keyval[0] == "letter"){
                        //    f.letter = keyval[1].substr (1, (unsigned long) (keyval[1].length()-2));
                    }
                }
                fontMap[id] = fc;
            } else if (elems[0] == "kernings") {

            } else if (elems[0] == "kerning") {
                int first =0;
                int second=0;
                int amount=0;
                for (int i=1;i<elems.size();i++) {

                    split(elems[i], '=', keyval);
                    if (keyval[0] == "first"){
                        first  =stoi(keyval[1]);
                    } else if (keyval[0] == "second"){
                        second =stoi(keyval[1]);
                    } else if (keyval[0] == "amount"){
                        amount =stoi(keyval[1]);
                    }
                }
                kerning[pair<int,int>{first, second}] = amount;
            } else {
                cout << "Not found type "<<elems[0]<<endl;
            }
            elems.clear();
        }
        return true;
    }

    int Font::width(std::string text) {
        int sum = 0;
        int lastChar = -1;
        for (int i=0;i<text.length();i++){
            int f = text[i];
            auto found = fontMap.find(f);
            if (found != fontMap.end()){
                sum += getKerning(lastChar, f);
                sum += found->second.xadvance;
                lastChar = f;
            } else {
                cout << "Cannot find char "<<f<<endl;
                return -1;
            }
        }
        return sum;
    }

    int Font::getKerning(int t1, int t2){
        auto foundKerning = kerning.find(pair<int,int>{t1, t2});
        if (foundKerning != kerning.end()){
            return foundKerning->second;
        }
        return 0;
    }

    int Font::height() {
        return lineHeight;
    }

    shared_ptr<Texture2D> Font::getTexture() const {
        return texture;
    }

    void Font::setTexture(shared_ptr<Texture2D> texture) {
        Font::texture = texture;
        fontListener.notifyListeners(this);
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

    void Font::setShader(std::shared_ptr<Shader> shader) {
        this->shader = shader;
        fontListener.notifyListeners(this);
    }

    std::shared_ptr<Shader> Font::getShader() {
        return shader;
    }

    bool Font::IsInitialized() const {
        return texture != nullptr;
    }
}