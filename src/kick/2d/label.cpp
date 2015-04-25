//
// Created by morten on 30/06/14.
//

#include "label.h"
#include <iostream>
#include "kick/mesh/mesh.h"
#include "glm/gtx/string_cast.hpp"

using namespace std;
using namespace glm;

namespace kick {

    Label::Label(GameObject *gameObject)
            : Component2D(gameObject),
              bounds{vec2{0,0},vec2{0,0}} {
        mesh = new Mesh();
        meshData = make_shared<MeshData>();
        material = new Material();
        mesh->setMeshData(meshData);
    }

    std::string const &Label::getText() const {
        return text;
    }

    void Label::setText(std::string const &text) {
        Label::text = text;
        updateVertexBuffer();
    }

    void Label::render(EngineUniforms *engineUniforms) {
        if (!font) return;
        auto shader = material->shader();
        assert(shader);
        mesh->bind(shader.get());
        shader->bind_uniforms(material, engineUniforms, transform());
        mesh->render(0);
    }

    void Label::setFont(std::shared_ptr<Font>& font) {
        if (!font->initialized()){
            font->loadFntFile();
        }
        Label::font = font;
        auto setupFont = [&](Font* f){
            material->setShader(f->getShader());
            material->setUniform("mainTexture", f->getTexture());
        };
        setupFont(font.get());
        eventListener = font->fontListener.createListener(setupFont);
        updateVertexBuffer();
    }

    std::shared_ptr<Font> Label::getFont() const {
        return font;
    }

    void Label::updateVertexBuffer() {
        if (!font || text.length() ==0) {
            meshData->setSubmesh(0, {}, MeshType::Triangles);
            mesh->setMeshData(meshData);
            return;
        }
        vector<vec3> position;
        vector<vec2> textureCoords;
        vector<GLushort> indices;
        vec2 caret{-font->width(text) * anchor.x, -font->height() * anchor.y};

        if (text.length()>0){ // if first char is x-offset, then undo offset to fix horizontal alignment
            const FontChar fc = font->getChar(text[0]);
            caret.x -= fc.xoffset;
        }

        bounds.max = vec2{-FLT_MAX};
        bounds.min = vec2{FLT_MAX};
        GLushort vertexCount = 0;
        int lastChar = -1;
        for (unsigned short i=0;i<text.length();i++){
            const FontChar fc = font->getChar(text[i]);
            if (fc.width  <= 0){
                continue;
            }
            caret.x += font->getKerning(lastChar, text[i]);
            vec2 basePoint = caret + vec2{fc.xoffset,font->height()-fc.height-fc.yoffset};
            position.push_back(vec3{basePoint,0});
            position.push_back(vec3{basePoint+vec2{fc.width,0},0});
            position.push_back(vec3{basePoint+vec2{fc.width,fc.height},0});
            position.push_back(vec3{basePoint+vec2{0       ,fc.height},0});

            bounds.min = glm::min(bounds.min, basePoint);
            bounds.max = glm::max(bounds.max, (vec2)position[position.size()-2]);

            vec2 scale{1.0f / font->getScaleW(), 1.0f / font->getScaleH()};

            textureCoords.push_back(vec2{fc.x         ,font->getScaleH()-fc.y-fc.height}*scale);
            textureCoords.push_back(vec2{fc.x+fc.width,font->getScaleH()-fc.y-fc.height}*scale);
            textureCoords.push_back(vec2{fc.x+fc.width,font->getScaleH()-fc.y}*scale);
            textureCoords.push_back(vec2{fc.x         ,font->getScaleH()-fc.y}*scale);

            // push two triangles
            indices.push_back(vertexCount);
            indices.push_back(vertexCount + (GLushort)1);
            indices.push_back(vertexCount + (GLushort)2);
            indices.push_back(vertexCount + (GLushort)2);
            indices.push_back(vertexCount + (GLushort)3);
            indices.push_back(vertexCount);

            vertexCount += 4;

            caret.x += fc.xadvance;
            lastChar = text[i];
        }

        meshData->setPosition(position);
        meshData->setTexCoord0(textureCoords);
        meshData->setSubmesh(0,indices, MeshType::Triangles);
        mesh->setMeshData(meshData);
    }

    Bounds2 Label::getBounds() {
        return bounds;
    }

    Material *Label::getMaterial() const {
        return material;
    }

    void Label::setMaterial(Material *material) {
        assert(material);
        Label::material = material;
        material->setShader(font->getShader());
        material->setUniform("mainTexture", font->getTexture());
    }

    int Label::getRenderOrder() {
        return material->renderOrder();
    }

    void Label::setAnchor(glm::vec2 anchor) {
        if (anchor != Label::anchor){
            Label::anchor = anchor;
            updateVertexBuffer();
        }
    }

    glm::vec2 Label::getAnchor() const {
        return anchor;
    }

    Shader *Label::getShader() const {
        if (material){
            return material->shader().get();
        }
        return nullptr;
    }
}