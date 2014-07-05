//
// Created by morten on 30/06/14.
//

#include "text.h"
#include <iostream>
#include "kick/mesh/mesh.h"
#include "glm/gtx/string_cast.hpp"
using namespace std;
using namespace glm;

namespace kick {

    Text::Text(GameObject *gameObject) : ComponentRenderable(gameObject) {
        mesh = Project::createAsset<Mesh>();
        meshData = Project::createAsset<MeshData>();
        mesh->setMeshData(meshData);
    }

    std::string const &Text::getText() const {
        return text;
    }

    void Text::setText(std::string const &text) {
        Text::text = text;
        updateVertexBuffer();

    }

    void Text::render(EngineUniforms *engineUniforms) {
        printOpenGLError();
        if (!font) return;
        auto material = font->material;
        auto shader = material->getShader();
        printOpenGLError();
        mesh->bind(shader);
        printOpenGLError();
        shader->bind_uniforms(material, engineUniforms, getTransform());
        printOpenGLError();
        mesh->render(0);
        printOpenGLError();
    }

    void Text::setFont(Font *font) {
        Text::font = font;
        updateVertexBuffer();
    }

    Font *Text::getFont() const {
        return font;
    }

    void Text::updateVertexBuffer() {
        if (!font || text.length() ==0) {
            meshData->setSubmesh(0, {}, MeshType::Triangles);
            mesh->setMeshData(meshData);
            return;
        }
        vector<vec3> position;
        vector<vec2> textureCoords;
        vector<GLuint> indices;
        vec2 carent{0,0};
        int halfLineHeight = font->height()/2;
        for (unsigned int i=0;i<text.length();i++){
            FontChar fc = font->getChar(text[i]);
            if (fc.width  == -1){
                continue;
            }
            vec2 basePoint = carent+vec2{fc.xoffset,halfLineHeight-fc.height-fc.yoffset};

            position.push_back(vec3{basePoint,0});
            position.push_back(vec3{basePoint+vec2{fc.width,0},0});
            position.push_back(vec3{basePoint+vec2{fc.width,fc.height},0});
            position.push_back(vec3{basePoint+vec2{0,fc.height},0});

            vec2 scale{1.0f/font->getScaleW(), 1.0f/font->getScaleH()};

            textureCoords.push_back(vec2{fc.x,font->getScaleH()-fc.y-fc.height}*scale);
            textureCoords.push_back(vec2{fc.x+fc.width,font->getScaleH()-fc.y-fc.height}*scale);
            textureCoords.push_back(vec2{fc.x+fc.width,font->getScaleH()-fc.y}*scale);
            textureCoords.push_back(vec2{fc.x,font->getScaleH()-fc.y}*scale);

            // push two triangles
            indices.push_back(i*4);
            indices.push_back(i*4+1);
            indices.push_back(i*4+2);
            indices.push_back(i*4+2);
            indices.push_back(i*4+3);
            indices.push_back(i*4);

            carent.x += fc.xadvance;
        }
        meshData->setPosition(position);
        meshData->setTexCoord0(textureCoords);
        meshData->setSubmesh(0,indices, MeshType::Triangles);
        mesh->setMeshData(meshData);
    }
}