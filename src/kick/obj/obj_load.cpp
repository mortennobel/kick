//
//  ObjLoader.cpp
//  KickObjLoader
//
//  Created by morten on 8/31/13.
//  Copyright (c) 2013 morten. All rights reserved.
//

#include "obj_load.h"
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <cerrno>
#include "glm/ext.hpp"
#include "kick/core/log.h"

using namespace std;
using namespace glm;

namespace kick {
    // from http://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
    string getFileContents(string filename)
    {
        ifstream in{filename, ios::in | ios::binary};
        if (in)
        {
            std::string contents;
            in.seekg(0, std::ios::end);
            contents.resize(in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(&contents[0], contents.size());
            in.close();
            return contents;
        }
        logError(string{"Error loading file "}+filename+" error "+std::to_string(errno));
        return "";
    }
    
    vec4 toVec4(vector<string> &tokens){
        vec4 res{0,0,0,1};
        for (int i=0;i<4;i++){
            if (i+1 < tokens.size()){
                res[i] = atof(tokens[i+1].c_str());
            }
        }
        return res;
    }
    
    vec3 toVec3(vector<string> &tokens){
        vec3 res{0,0,0};
        for (int i=0;i<3;i++){
            if (i+1 < tokens.size()){
                res[i] = atof(tokens[i+1].c_str());
            }
        }
        return res;
    }
    
    bool replace(std::string& str, const std::string& from, const std::string& to) {
        size_t start_pos = str.find(from);
        if(start_pos == std::string::npos)
            return false;
        str.replace(start_pos, from.length(), to);
        return true;
    }
    
    ObjFace toObjFace(vector<string> &tokens){
        ObjFace res;
        for (int i=1; i<tokens.size(); i++) {
            string & p = tokens[i];
            replace(p, "//","/0/");
            
            ObjVertex o{0,0,0};
            
            stringstream ss{p};
            char buffer[50];
            
            ss.getline(buffer,50, '/');
            o.vertexPositionIdx = atoi(buffer);
            if (ss.good()){
                ss.getline(buffer,50, '/');
                o.textureIdx = atoi(buffer);
            }
            if (ss.good()){
                ss.getline(buffer,50, '/');
                o.normalIdx = atoi(buffer);
            }
            res.push_back(o);
        }
        return move(res);
    }
    
    void parseMaterialLib(std::string & materialLib, ObjData & dest){
        const int bufferSize = 256;
        char buffer[bufferSize];
        char buffer2[bufferSize];
        stringstream ss{materialLib};
        
        while (ss.good()){
            ss.getline(buffer, bufferSize);
            stringstream likeTokensizer{buffer};
            vector<string> tokens;
            while (likeTokensizer.good()) {
                likeTokensizer.getline(buffer2, bufferSize, ' ');
                tokens.push_back(buffer2);
            }
            if (tokens.size()==0){
                continue;
            }
            if (tokens[0] == "newmtl"){
                vec3 zero{0,0,0};
                ObjMaterial material{tokens[1],zero,zero,zero,50,1};
                dest.materials.push_back(material);
            } else {
                if (dest.materials.size()==0){
                    continue;
                }
                ObjMaterial & currentMat = dest.materials.back();
                if (tokens[0] == "Ka"){
                    currentMat.ambientColor = toVec3(tokens);
                } else if (tokens[0] == "Kd"){
                    currentMat.diffuseColor = toVec3(tokens);
                } else if (tokens[0] == "Ks"){
                    currentMat.specularColor = toVec3(tokens);
                } else if (tokens[0] == "d"){
                    currentMat.transparent = atoi(tokens[1].c_str());
                } else if (tokens[0] == "illum"){
                    int illumMode = atoi(tokens[1].c_str());
                    currentMat.illuminationModes.push_back(static_cast<ObjIlluminationMode>(illumMode));
                } else if (tokens[0] == "map_Ka"){
                    
                } else if (tokens[0] == "map_Kd"){
                } else if (tokens[0] == "map_Ks"){
                } else if (tokens[0] == "map_Ns"){
                } else if (tokens[0] == "map_d"){
                } else if (tokens[0] == "map_bump" || tokens[0] == "bump" ){
                } else if (tokens[0] == "map_disp" || tokens[0] == "disp" ){
                } else if (tokens[0] == "map_decal" || tokens[0] == "decal" ){
                }
            }
        }
    }
    
    ObjData loadObjData(std::string objSource, std::function<std::string (std::string)> materialMap){
        ObjData res;
        stringstream ss{objSource};
        const int bufferSize = 256;
        char buffer[bufferSize];
        char buffer2[bufferSize];
        bool initialComment = true;
        while (ss.good()){
            ss.getline(buffer, bufferSize);
            stringstream likeTokensizer{buffer};
            vector<string> tokens;
            while (likeTokensizer.good()) {
                likeTokensizer.getline(buffer2, bufferSize, ' ');
                tokens.push_back(buffer2);
            }
            if (tokens.size()==0){
                continue;
            }
            initialComment = initialComment && (tokens[0] == "#");
            int currentIndex = static_cast<int>(res.faces.size()) + 1;
            if (tokens[0] == "#"){ // comment
                if (initialComment){
                    res.initialComment += string(buffer + 2) + "\n";
                }
            } else if (tokens[0] == "v"){ // vertex position
                vec4 vertexPosition = toVec4(tokens);
                res.vertexPositions.push_back(vertexPosition);
            } else if (tokens[0] == "vt"){ // vertex texture coordinates
                vec3 textureCoord = toVec3(tokens);
                res.textureCoords.push_back(textureCoord);
            } else if (tokens[0] == "vn"){ // vertex normal
                vec3 normal = toVec3(tokens);
                res.normals.push_back(normal);
            } else if (tokens[0] == "f"){ // face
                res.faces.push_back(toObjFace(tokens));
            } else if (tokens[0] == "mtllib"){ // material library
                string materialLib = materialMap(tokens[1]);
                parseMaterialLib(materialLib, res);
            } else if (tokens[0] == "usemtl"){ // use material
                res.materialChanges.push_back({currentIndex, tokens[1]});
            } else if (tokens[0] == "o"){ // named object
                res.namedObjects.push_back({currentIndex, tokens[1]});
            } else if (tokens[0] == "g"){ // polygon group
                res.polygonGroups.push_back({currentIndex, tokens[1]});
            } else if (tokens[0] == "s"){ // smoothing groups
                int smoothingGroup = 0; // 0 = no smoothing
                if (tokens[1] != "off"){
                    smoothingGroup = atoi(tokens[1].c_str());
                }
                res.smoothGroups.push_back({currentIndex, smoothingGroup});
            }
        }

        return res;
    }
    
    std::string fixPathEnd(std::string &path){
        char lastChar = path[path.length()-1];
        if (lastChar != '/'){
            return path + "/";
        }
        return path;
    }
    
    ObjData loadObjData(std::string path, std::string filename){
        path = fixPathEnd(path);
        string file = getFileContents(path+filename);
        std::function<std::string (std::string)>  materialLoader = [&](string filename){
            return getFileContents(path+filename);
        };
        return loadObjData(file, materialLoader);
    }
}