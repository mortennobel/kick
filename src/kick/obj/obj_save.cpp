//
//  ObjSave.cpp
//  KickObjLoader
//
//  Created by morten on 9/1/13.
//  Copyright (c) 2013 morten. All rights reserved.
//

#include "obj_save.h"
#include "obj_load.h"
#include <fstream>
#include <sstream>
#include <cstring>

using namespace std;
using namespace glm;

namespace kick {
    
    ostream& operator<<(ostream& cout,vec3 obj)
    {
        cout << obj[0]<<" "<<obj[1] << " "<<obj[2];
        return cout;
    }

    ostream& operator<<(ostream& cout,vec4 obj)
    {
        cout << obj[0]<<" "<<obj[1] << " "<<obj[2] << " "<<obj[3];
        return cout;
    }
    
    ostream& operator<<(ostream& cout,ObjVertex obj)
    {
        cout << " " << obj.vertexPositionIdx;
        if (obj.textureIdx>0){
            cout << "/"<<obj.textureIdx;
            if (obj.normalIdx>0){
                cout << "/"<<obj.normalIdx;
            }
        } else if (obj.normalIdx>0){
            cout << "//"<<obj.normalIdx;
        }
        return cout;
    }
    
    
    string saveMaterialLib(ObjData & data, std::string path, std::string filename){
        size_t pos = filename.find_last_of('.');
        if (pos != string::npos){
            filename = filename.substr(0, pos);
        }
        filename += ".mtl";
        
        ofstream file;
        file.open ((path + filename).c_str());
        for (auto & material : data.materials){
            file << "newmtl "<<material.name<<endl;
            file << "Ns "<<material.specularCoefficient<<endl;
            file << "Ka "<<material.ambientColor<<endl;
            file << "Kd "<<material.diffuseColor<<endl;
            file << "Ks "<<material.specularColor<<endl;
            file << "d "<<material.transparent<<endl;
            for (auto & i : material.illuminationModes) {
                file << "illum "<<static_cast<int>(i)<<endl;
            }
        }
        file.close();
        
        return filename;
    }
    
    void saveObjData(ObjData & data, std::string path, std::string filename){
        path = fixPathEnd(path);
        ofstream file;
        file.open ((path + filename).c_str());
        // save initial comment
        stringstream ss{data.initialComment};
        while (ss.good()){
            const int bufferSize = 256;
            char buffer[bufferSize];
            ss.getline(buffer, bufferSize);
            if (strlen(buffer)>0){
                file << "# "<< buffer << endl;
            }
        }
        if (!data.materials.empty()){
            string name = saveMaterialLib(data, path, filename);
            file << "mtllib "<< name << endl;
        }
        for (auto & vertexPos: data.vertexPositions) {
            file << "v ";
            if (vertexPos[3] == 1){ // remove optional 4th value if possible
                vec3 v{vertexPos[0],vertexPos[1],vertexPos[2]};
                 file << v << endl;
            } else {
                file << vertexPos<<endl;
            }
        }
        for (auto & texCoord: data.textureCoords) {
            file << "vt ";
            if (texCoord[2] == 0){ // remove optional 3 coord if possible
                file << texCoord[0] << " "<< texCoord[1]<<endl;
            } else {
                file << texCoord << endl;
            }
        }
        for (auto & normal: data.normals) {
            file << "vn " << normal << endl;
        }
        int faceIndex = 1;
        auto materialIter = data.materialChanges.begin();
        auto materialIterEnd = data.materialChanges.end();
        auto smoothingGroupIter = data.smoothGroups.begin();
        auto smoothingGroupIterEnd = data.smoothGroups.end();
        for (auto & face : data.faces) {
            while (materialIter != materialIterEnd && materialIter->faceIndex==faceIndex){
                file << "usemtl "<<materialIter->name<<endl;;
                materialIter++;
            }
            while (smoothingGroupIter != smoothingGroupIterEnd && smoothingGroupIter->faceIndex==faceIndex){
                int group = smoothingGroupIter->smoothGroupIdx;
                file << "s ";
                if (group == 0){
                    file << "off";
                } else {
                    file << group;
                }
                file<<endl;;
                smoothingGroupIter++;
            }
            file << "f";
            for (auto & vertexIndex : face) {
                file << vertexIndex;
            }
            file << endl;
            faceIndex++;
        }
        
        file.close();
    }
}