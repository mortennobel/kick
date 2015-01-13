//
//  ObjLoader.cpp
//  KickObjLoader
//
//  Created by morten on 8/31/13.
//  Copyright (c) 2013 morten. All rights reserved.
//

#include "kick/ply/ply_load.h"
#include "kick/core/project.h"
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <cerrno>
#include "glm/ext.hpp"
#include "kick/core/debug.h"

using namespace std;
using namespace glm;

namespace kick {

    namespace {

        enum class VertexAttributeType {
            Pos,
            Normal,
            Color,
            Unknown
        };
        enum class VertexAttributeDataType {
            v_char,//       character                 1
                    v_uchar,//      unsigned character        1
                    v_short,//      short integer             2
                    v_ushort,//     unsigned short integer    2
                    v_int,//        integer                   4
                    v_uint,//       unsigned integer          4
                    v_float,//      single-precision float    4
                    v_double,//     double-precision float    8
                    v_unknown
        };

        VertexAttributeDataType toType(string s){
            if (s=="char"){
                return VertexAttributeDataType::v_char;
            }
            if (s=="uchar"){
                return VertexAttributeDataType::v_uchar;
            }
            if (s=="short"){
                return VertexAttributeDataType::v_short;
            }
            if (s=="ushort"){
                return VertexAttributeDataType::v_ushort;
            }
            if (s=="int"){
                return VertexAttributeDataType::v_int;
            }
            if (s=="uint"){
                return VertexAttributeDataType::v_uint;
            }
            if (s=="float"){
                return VertexAttributeDataType::v_float;
            }
            if (s=="double"){
                return VertexAttributeDataType::v_double;
            }
            logWarning("Unknown type "+s);
            return VertexAttributeDataType::v_unknown;
        }

        struct VertexMapping{
            VertexAttributeType type;
            VertexAttributeDataType dataType;
        };

        // from http://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
        /*string getFileContents(string filename)
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
        }  */

        std::string fixPathEnd(std::string &path){
            if (path.length() == 0){
                return path;
            }
            char lastChar = path[path.length()-1];
            if (lastChar != '/'){
                return path + "/";
            }
            return path;
        }

        vec3 toVec3(vector<string> &tokens, int offset, float div=1){
            vec3 res{0,0,0};
            for (int i=0;i<3;i++){
                if (i+offset < tokens.size()){
                    res[i] = atof(tokens[i+offset].c_str())/div;
                }
            }
            return res;
        }

//        ObjFace toObjFace(vector<string> &tokens){
//            ObjFace res;
//            for (int i=1; i<tokens.size(); i++) {
//                string & p = tokens[i];
//                replace(p, "//","/0/");
//
//                if (p.length()==0){
//                    continue;
//                }
//
//                ObjVertex o{0,0,0};
//
//                stringstream ss{p};
//                char buffer[50];
//                ss.getline(buffer,50, '/');
//                o.vertexPositionIdx = atoi(buffer);
//                if (ss.good()){
//                    ss.getline(buffer,50, '/');
//                    o.textureIdx = atoi(buffer);
//                }
//                if (ss.good()){
//                    ss.getline(buffer,50, '/');
//                    o.normalIdx = atoi(buffer);
//                }
//                res.push_back(o);
//            }
//            return res;
//        }


    }

    std::shared_ptr<MeshData> loadPlyData(std::string objSource){
        auto res = make_shared<MeshData>();
        stringstream ss{objSource};
        const int bufferSize = 256;
        char buffer[bufferSize];
        char buffer2[bufferSize];
        bool headerEnded = false;
        int vertices = -1;
        int faces = -1;
        int count = 0;

        vector<vec3> vertexPos;
        vector<vec3> normals;
        vector<vec4> colors;
        vector<unsigned short> indices;

        vector<VertexMapping> mapping;

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
            if (tokens[0] == "ply"){

            } else if (tokens[0] == "format"){
                if (tokens[1] != "ascii"){
                    logWarning("Only PLY ascii is supported");
                }
            } else if (tokens[0] == "element"){
                if (tokens[1] == "vertex"){
                    vertices = atoi(tokens[2].c_str());
                } else if (tokens[1] == "face"){
                    faces = atoi(tokens[2].c_str());
                }
            } else if (tokens[0] == "property"){
                if (tokens[2]=="x"){
                    mapping.push_back({VertexAttributeType::Pos,toType(tokens[1])});
                } else if (tokens[2]=="y" || tokens[2]=="z"){

                } else if (tokens[2]=="nx"){
                    mapping.push_back({VertexAttributeType::Normal,toType(tokens[1])});
                } else if (tokens[2]=="ny" || tokens[2]=="nz"){

                } else if (tokens[2]=="red"){
                    mapping.push_back({VertexAttributeType::Color,toType(tokens[1])});
                } else if (tokens[2]=="green" || tokens[2]=="blue"){
                } else if (tokens[1]=="list"){

                } else {
                    mapping.push_back({VertexAttributeType::Unknown,toType(tokens[1])});
                }
            } else if (tokens[0] == "end_header"){
                headerEnded = true;
                if (vertices == -1){
                    logWarning("Undefined vertex count");
                }
                if (faces == -1){
                    logWarning("Undefined face count");
                }
            } else if (headerEnded){
                count++;
                bool isVertex = count <= vertices;
                if (isVertex){
                    int idx = 0;
                    for (auto t : mapping){
                        if (t.type == VertexAttributeType::Pos){
                            vertexPos.push_back(toVec3(tokens, idx));
                        }
                        else if (t.type == VertexAttributeType::Normal){
                            normals.push_back(toVec3(tokens, idx));
                        } else if (t.type == VertexAttributeType::Color) {
                            colors.push_back(vec4(toVec3(tokens, idx, 255),1));
                        }
                            idx += (t.type == VertexAttributeType::Unknown) ? 1 : 3;
                    }
                } else {
                    int count = atoi(tokens[0].c_str());
                    for (int i=0;i<count;i++){
                        if (i>2){
                            // add first and last node to do simple triangulation
                            indices.push_back(atoi(tokens[1].c_str()));
                            indices.push_back(atoi(tokens[i].c_str()));
                        }
                        indices.push_back(atoi(tokens[i+1].c_str()));
                    }

                }
            }
        }
        res->setPosition(vertexPos);
        if (normals.size()>0){
            res->setNormal(normals);
        }
        if (colors.size()>0){
            res->setColor(colors);
        }
        res->setSubmesh(0, indices, MeshType::Triangles);
        return res;
    }


    std::shared_ptr<MeshData> loadPlyData(std::string path, std::string filename){
        path = fixPathEnd(path);

        string fileSrc;// = getFileContents(path+filename);
        Project::loadTextResource(path+filename,fileSrc);
        return loadPlyData(fileSrc);
    }
}