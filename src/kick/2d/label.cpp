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
              mBounds{vec2{0,0},vec2{0,0}} {
        mMesh = new Mesh();
        mMeshData = make_shared<MeshData>();
        mMaterial = new Material();
        mMesh->setMeshData(mMeshData);
    }

    std::string const &Label::text() const {
        return mText;
    }

    void Label::setText(std::string const &text) {
        Label::mText = text;
        updateVertexBuffer();
    }

    void Label::render(EngineUniforms *engineUniforms) {
        if (!mFont) return;
        auto shader = mMaterial->shader();
        assert(shader);
        mMesh->bind(shader.get());
        shader->bind_uniforms(mMaterial, engineUniforms, transform().get());
        mMesh->render(0);
    }

    void Label::setFont(std::shared_ptr<Font>& font) {
        if (!font->initialized()){
            font->loadFntFile();
        }
        Label::mFont = font;
        auto setupFont = [&](Font* f){
            mMaterial->setShader(f->getShader());
            mMaterial->setUniform("mainTexture", f->texture());
        };
        setupFont(font.get());
        mEventListener = font->fontListener.createListener(setupFont);
        updateVertexBuffer();
    }

    std::shared_ptr<Font> Label::font() const {
        return mFont;
    }

    void Label::updateVertexBuffer() {
        if (!mFont || mText.length() ==0) {
            mMeshData->setSubmesh(0, {}, MeshType::Triangles);
            mMesh->setMeshData(mMeshData);
            return;
        }
        vector<vec3> position;
        vector<vec2> textureCoords;
        vector<GLushort> indices;
        vec2 caret{-mFont->width(mText) * mAnchor.x, -mFont->height() * mAnchor.y};

        if (mText.length()>0){ // if first char is x-offset, then undo offset to fix horizontal alignment
            const FontChar fc = mFont->getChar(mText[0]);
            caret.x -= fc.xoffset;
        }

        mBounds.max = vec2{-FLT_MAX};
        mBounds.min = vec2{FLT_MAX};
        GLushort vertexCount = 0;
        int lastChar = -1;
        for (unsigned short i=0;i< mText.length();i++){
            const FontChar fc = mFont->getChar(mText[i]);
            if (fc.width  <= 0){
                continue;
            }
            caret.x += mFont->getKerning(lastChar, mText[i]);
            vec2 basePoint = caret + vec2{fc.xoffset, mFont->height()-fc.height-fc.yoffset};
            position.push_back(vec3{basePoint,0});
            position.push_back(vec3{basePoint+vec2{fc.width,0},0});
            position.push_back(vec3{basePoint+vec2{fc.width,fc.height},0});
            position.push_back(vec3{basePoint+vec2{0       ,fc.height},0});

            mBounds.min = glm::min(mBounds.min, basePoint);
            mBounds.max = glm::max(mBounds.max, (vec2)position[position.size()-2]);

            vec2 scale{1.0f / mFont->scaleW(), 1.0f / mFont->scaleH()};

            textureCoords.push_back(vec2{fc.x         , mFont->scaleH()-fc.y-fc.height}*scale);
            textureCoords.push_back(vec2{fc.x+fc.width, mFont->scaleH()-fc.y-fc.height}*scale);
            textureCoords.push_back(vec2{fc.x+fc.width, mFont->scaleH()-fc.y}*scale);
            textureCoords.push_back(vec2{fc.x         , mFont->scaleH()-fc.y}*scale);

            // push two triangles
            indices.push_back(vertexCount);
            indices.push_back(vertexCount + (GLushort)1);
            indices.push_back(vertexCount + (GLushort)2);
            indices.push_back(vertexCount + (GLushort)2);
            indices.push_back(vertexCount + (GLushort)3);
            indices.push_back(vertexCount);

            vertexCount += 4;

            caret.x += fc.xadvance;
            lastChar = mText[i];
        }

        mMeshData->setPosition(position);
        mMeshData->setTexCoord0(textureCoords);
        mMeshData->setSubmesh(0,indices, MeshType::Triangles);
        mMesh->setMeshData(mMeshData);
    }

    Bounds2 Label::bounds() const {
        return mBounds;
    }

    Material *Label::material() const {
        return mMaterial;
    }

    void Label::setMaterial(Material *material) {
        assert(material);
        Label::mMaterial = material;
        material->setShader(mFont->getShader());
        material->setUniform("mainTexture", mFont->texture());
    }

    int Label::renderOrder() {
        return mMaterial->renderOrder();
    }

    void Label::setAnchor(glm::vec2 anchor) {
        if (anchor != Label::mAnchor){
            Label::mAnchor = anchor;
            updateVertexBuffer();
        }
    }

    glm::vec2 Label::anchor() const {
        return mAnchor;
    }

    Shader *Label::shader() const {
        if (mMaterial){
            return mMaterial->shader().get();
        }
        return nullptr;
    }

    void Label::setBounds(Bounds2 bounds) {
        cout << "Implement" << endl;
    }
}