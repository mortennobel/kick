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

    Text::Text(GameObject *gameObject)
            : ComponentRenderable(gameObject),
              bounds{vec2{0,0},vec2{0,0}} {
        mesh = Project::createAsset<Mesh>();
        meshData = Project::createAsset<MeshData>();
        material = Project::createAsset<Material>();
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
        if (!font) return;
        auto shader = material->getShader();
        assert(shader);
        mesh->bind(shader);
        shader->bind_uniforms(material, engineUniforms, getTransform());
        mesh->render(0);
    }

    void Text::setFont(Font *font) {
        if (!font->IsInitialized()){
            font->loadFntFile();
        }
        Text::font = font;
        material->setShader(font->getShader());
        material->setUniform("mainTexture", font->getTexture());
        eventListener = font->fontListener.createListener([&](Font* f){
            material->setShader(f->getShader());
            material->setUniform("mainTexture", f->getTexture());
        });
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
        vector<GLushort> indices;
        vec2 carent{0,0};
        bounds.max = vec2{-FLT_MAX};
        bounds.min = vec2{FLT_MAX};
        for (unsigned short i=0;i<text.length();i++){
            FontChar fc = font->getChar(text[i]);
            if (fc.width  == -1){
                continue;
            }
            vec2 basePoint = carent+vec2{fc.xoffset,font->height()-fc.height-fc.yoffset};

            position.push_back(vec3{basePoint,0});
            position.push_back(vec3{basePoint+vec2{fc.width,0},0});
            position.push_back(vec3{basePoint+vec2{fc.width,fc.height},0});
            position.push_back(vec3{basePoint+vec2{0,fc.height},0});

            bounds.min = glm::min(bounds.min, basePoint);
            bounds.max = glm::max(bounds.max, (vec2)position[position.size()-2]);

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

    Bounds2D Text::getBounds() {
        return bounds;
    }

    Material *Text::getMaterial() const {
        return material;
    }

    void Text::setMaterial(Material *material) {
        assert(material);
        Text::material = material;
        material->setShader(font->getShader());
        material->setUniform("mainTexture", font->getTexture());
    }

    int Text::getRenderOrder() {
        return material->getRenderOrder();
    }
}