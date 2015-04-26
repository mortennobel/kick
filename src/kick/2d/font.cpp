//
// Created by morten on 30/06/14.
//

#include "kick/2d/font.h"
#include "kick/core/project.h"
#include "kick/core/debug.h"
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

namespace kick{
    Font::Font()  {
        fShader = Project::loadShader("assets/shaders/font.shader");
    }

    Font::~Font() {
    }

    std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
        elems.clear();
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
			if (item.length()>0){
				elems.push_back(item);
			}
        }
        return elems;
    }

    bool Font::loadFntFile(string filename) {
        string texturename = filename.substr(0, filename.size()-4) + ".png";
        mFontMap.clear();
        mKerning.clear();
        TextureSampler sampler;
        sampler.filterMag = TextureFilter::Linear;
        sampler.filterMin = TextureFilter::LinearMipmapLinear;

        setTexture(Project::loadTexture2D(texturename, sampler));
        if (!mTexture){
            logWarning(string("Cannot load font texture file ")+texturename);
            return false;
        }
        string fntFile;
        if (!Project::loadTextResource(filename,fntFile)){
            logWarning(string("Cannot load font text file ")+filename);
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
                        mLineHeight = stoi(keyval[1]);
                    } else if (keyval[0] == "base"){
                        mBase = stoi(keyval[1]);
                    } else if (keyval[0] == "scaleW"){
                        mScaleW = stoi(keyval[1]);
                    } else if (keyval[0] == "scaleH"){
                        mScaleH = stoi(keyval[1]);
                    } else if (keyval[0] == "pages"){
                        mPages = stoi(keyval[1]);
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
                mFontMap[id] = fc;
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
                mKerning[pair<int,int>{first, second}] = amount;
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
            auto foundChar = mFontMap.find(f);
            if (foundChar != mFontMap.end()){
                sum += getKerning(lastChar, f);
                if (i == text.length()-1){
                    sum += foundChar->second.width;
                } else {
                    sum += foundChar->second.xadvance;
                }
                lastChar = f;
            } else {
                cout << "Cannot find char "<<f<<endl;
                return -1;
            }
        }
        return sum;
    }

    int Font::getKerning(int t1, int t2){
        auto foundKerning = mKerning.find(pair<int,int>{t1, t2});
        if (foundKerning != mKerning.end()){
            return foundKerning->second;
        }
        return 0;
    }

    int Font::height() {
        return mLineHeight;
    }

    shared_ptr<Texture2D> Font::texture() const {
        return mTexture;
    }

    void Font::setTexture(shared_ptr<Texture2D> texture) {
        Font::mTexture = texture;
        fontListener.notifyListeners(this);
    }

    const FontChar Font::getChar(char r) {
        auto found = mFontMap.find((int)r);
        if (found != mFontMap.end()){
            return found->second;
        } else {
            return kick::FontChar{0,0,0,0};
        }
    }

    int Font::scaleW() const {
        return mScaleW;
    }

    int Font::scaleH() const {
        return mScaleH;
    }

    int Font::base() const {
        return mBase;
    }

    void Font::setShader(std::shared_ptr<Shader> shader) {
        this->fShader = shader;
        fontListener.notifyListeners(this);
    }

    std::shared_ptr<Shader> Font::getShader() {
        return fShader;
    }

    bool Font::initialized() const {
        return mTexture != nullptr;
    }
}