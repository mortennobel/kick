//
// Created by morten on 26/07/14.
//

#include "canvas.h"
#include "label.h"
#include "kick/2d/sprite.h"
#include "kick/2d/button.h"
#include "kick/2d/toggle_button.h"
#include "kick/scene/scene.h"
#include "kick/2d/component2d.h"
#include "kick/material/material.h"
#include "kick/mesh/mesh.h"
#include "kick/mesh/mesh_data.h"
#include "glm/glm.hpp"
#include "kick/core/engine.h"
#include <algorithm>
#include <iostream>

using namespace kick;
using namespace std;
using namespace glm;

namespace kick{

    Canvas::Canvas(GameObject *gameObject) : ComponentRenderable(gameObject) {
        mMesh = new Mesh();
        mMeshData = make_shared<MeshData>();
        mMeshData->setMeshUsage(MeshUsage::DynamicDraw);
        mMesh->setMeshData(mMeshData);
        mMaterial = new Material();
        for (auto c : gameObject->componentsInChildren<Component2D>()){
            registerComponent2D(c);
        }
    }

    Canvas::~Canvas() {

    }

    void Canvas::render(kick::EngineUniforms *engineUniforms, Material* replacementMaterial) {
        if (!enabled()){
            return;
        }
        sort(mComponents.begin(), mComponents.end(), [](std::shared_ptr<Component2D> c1, std::shared_ptr<Component2D> c2){
            if (c1->order() != c2->order()){
                return c1->order() < c2->order();
            }
            return c1->shader() < c2->shader();
        });

        vector<Sprite*> sprites;
        TextureAtlas* textureAtlas = nullptr;
        for (auto& comp : mComponents){
            auto text = dynamic_pointer_cast<Label>(comp);
            if (text){
                renderSprites(sprites, engineUniforms, replacementMaterial); // render previous sprites
                text->render(engineUniforms);
            } else {
                auto sprite = dynamic_pointer_cast<Sprite>(comp);
                if (textureAtlas != sprite->textureAtlas().get()){
                    renderSprites(sprites, engineUniforms, replacementMaterial);
                    textureAtlas = sprite->textureAtlas().get();
                }
                if (sprite){
                    sprites.push_back(sprite.get());
                }
            }
        }
        renderSprites(sprites, engineUniforms, replacementMaterial);
    }

    void Canvas::updateVertexBuffer(std::vector<Sprite*> &sprites) {
        vector<vec3> position;
        vector<vec2> textureCoords;
        vector<vec4> colors;
        vector<GLushort> indices;
        sort(sprites.begin(), sprites.end(), [](Sprite* s1, Sprite* s2){
            return s1->order() < s2->order();
        });
        unsigned short index = 0;
        for (unsigned short i=0;i<sprites.size();i++){
            auto sprite = sprites[i];
            auto transform = sprite->transform();

            vec2 size = (vec2) sprite->textureAtlas()->textureSize();
            mat4 toWorld = transform->globalMatrix();
            vec2 scale = sprite->scale();


            TextureAtlasEntry entry = sprite->entry();
            Bounds2 bounds = sprite->trimmedBounds();
            vec4 color = sprite->color();

            if (sprite->type() == SpriteType::Simple) {
                position.push_back((vec3) (toWorld * vec4{bounds.lowLeft(), 0, 1}));
                position.push_back((vec3) (toWorld * vec4{bounds.lowRight(), 0, 1}));
                position.push_back((vec3) (toWorld * vec4{bounds.upperRight(), 0, 1}));
                position.push_back((vec3) (toWorld * vec4{bounds.upperLeft(), 0, 1}));

                vec2 min{entry.frame.x / size.x, 1.0 - (entry.frame.y + entry.frame.w) / size.y};
                vec2 max{(entry.frame.x + entry.frame.z) / size.x, 1.0 - entry.frame.y / size.y};
                textureCoords.push_back(vec2{min.x, min.y});
                textureCoords.push_back(vec2{max.x, min.y});
                textureCoords.push_back(vec2{max.x, max.y});
                textureCoords.push_back(vec2{min.x, max.y});

                colors.push_back(color);
                colors.push_back(color);
                colors.push_back(color);
                colors.push_back(color);

                // push two triangles
                indices.push_back(index);
                indices.push_back(index + 1);
                indices.push_back(index + 2);
                indices.push_back(index + 2);
                indices.push_back(index + 3);
                indices.push_back(index);
                index+=4;
            }
            else if (sprite->type() == SpriteType::Sliced) {
                vec2 dim {entry.frame.z,entry.frame.w};
                vec4 sliceX = vec4{bounds.min.x, bounds.min.x + dim.x * sprite->sliceX()[0],bounds.max.x - dim.x * (1.0- sprite->sliceX()[1]), bounds.max.x};
                vec4 sliceY = vec4{bounds.min.y, bounds.min.y + dim.y * sprite->sliceY()[0],bounds.max.y - dim.y * (1.0- sprite->sliceY()[1]), bounds.max.y};

                vec2 min{entry.frame.x / size.x, 1.0 - (entry.frame.y + entry.frame.w) / size.y};
                vec2 max{(entry.frame.x + entry.frame.z) / size.x, 1.0 - entry.frame.y / size.y};
                vec4 uvX{min.x, lerp(min.x, max.x, sprite->sliceX()[0]),lerp(min.x, max.x, sprite->sliceX()[1]),max.x};
                vec4 uvY{min.y, lerp(min.y, max.y, sprite->sliceY()[0]),lerp(min.y, max.y, sprite->sliceY()[1]),max.y};

                for (int x=0;x<3;x++){
                    for (int y=0;y<3;y++){
                        position.push_back((vec3) (toWorld * vec4{sliceX[x], sliceY[y], 0, 1}));
                        position.push_back((vec3) (toWorld * vec4{sliceX[x+1], sliceY[y], 0, 1}));
                        position.push_back((vec3) (toWorld * vec4{sliceX[x+1], sliceY[y+1], 0, 1}));
                        position.push_back((vec3) (toWorld * vec4{sliceX[x], sliceY[y+1], 0, 1}));

                        textureCoords.push_back(vec2{uvX[x], uvY[y]});
                        textureCoords.push_back(vec2{uvX[x+1], uvY[y]});
                        textureCoords.push_back(vec2{uvX[x+1], uvY[y+1]});
                        textureCoords.push_back(vec2{uvX[x], uvY[y+1]});

                        colors.push_back(color);
                        colors.push_back(color);
                        colors.push_back(color);
                        colors.push_back(color);

                        // push two triangles
                        indices.push_back(index);
                        indices.push_back(index + 1);
                        indices.push_back(index + 2);
                        indices.push_back(index + 2);
                        indices.push_back(index + 3);
                        indices.push_back(index);
                        index+=4;
                    }
                }
            }
        }
        mMeshData->setPosition(position);
        mMeshData->setColor(colors);
        mMeshData->setTexCoord0(textureCoords);
        mMeshData->setSubmesh(0,indices, MeshType::Triangles);
        mMesh->setMeshData(mMeshData);
    }

    void Canvas::renderSprites(vector<Sprite*> &sprites, kick::EngineUniforms *engineUniforms, Material* replacementMaterial) {
        if (sprites.size()==0){
            return;
        }
        updateVertexBuffer(sprites);

        auto mat = replacementMaterial ? replacementMaterial : mMaterial;
        if (!replacementMaterial ){
            mMaterial->setShader(sprites[0]->textureAtlas()->shader());
            mMaterial->setUniform("mainTexture", sprites[0]->textureAtlas()->texture());
        }
        auto shader = mat->shader();
        assert(shader);
        mMesh->bind(shader.get());

        shader->bind_uniforms(mMaterial, engineUniforms, transform().get());

        mMesh->render(0);

        sprites.clear();
    }

    int Canvas::renderOrder() {
        return 0;
    }

    std::shared_ptr<Camera> Canvas::camera() const {
        return mCamera;
    }

    void Canvas::setCamera(std::shared_ptr<Camera> camera) {
        Canvas::mCamera = camera;
        mGameObject->setLayer(256);
    }

    void Canvas::deactivated() {
        for (auto c : mComponents){
            deregisterComponent2D(c);
        }
    }

    void Canvas::registerComponent2D(std::shared_ptr<Component2D> comp) {
        mComponents.push_back(comp);
        comp->mCanvas = dynamic_pointer_cast<Canvas>(shared_from_this());


        auto sml = dynamic_pointer_cast<SpriteMouseListener>(comp);
        if (sml){
            mMouseListeners.push_back(sml);
        }
    }

    void Canvas::deregisterComponent2D(std::shared_ptr<Component2D> comp) {
        auto pos = find(mComponents.begin(), mComponents.end(), comp);
        if (pos != mComponents.end()){
            (*pos)->mCanvas = nullptr;
            mComponents.erase(pos);

            auto sml = dynamic_pointer_cast<SpriteMouseListener>(comp);
            auto pos2 = find(mMouseListeners.begin(), mMouseListeners.end(), sml);
            if (pos2 != mMouseListeners.end()){
                mMouseListeners.erase(pos2);
            }
        }
    }

    std::shared_ptr<Sprite> Canvas::createSprite(std::shared_ptr<TextureAtlas> textureAtlas, std::string spriteName, glm::vec2 pos) {
        GameObject *gameObject_ = gameObject()->scene()->createGameObject("Sprite");
        gameObject_->transform()->setParent(transform());
        auto sprite = gameObject_->addComponent<Sprite>();
        registerComponent2D(sprite);
        sprite->setTextureAtlas(textureAtlas);
        sprite->setSpriteName(spriteName);
        return sprite;
    }

    std::shared_ptr<Button> Canvas::createButton(std::string text) {
        std::shared_ptr<TextureAtlas> textureAtlas = Project::loadTextureAtlas("assets/ui/ui.txt");
        GameObject *gameObject_ = gameObject()->scene()->createGameObject("Button");
        gameObject_->transform()->setParent(transform());
        auto button = gameObject_->addComponent<Button>();
        registerComponent2D(button);
        button->setText("Button");
        button->setTextureAtlas(textureAtlas);
        button->setNormalSprite("button-normal.png");
        button->setHoverSprite("button-hover.png");
        button->setPressedSprite("button-pressed.png");
        button->setScale({2,2});
        button->setText(text);

        return button;
    }

    std::shared_ptr<Label> Canvas::createLabel(std::string text, int fontsize) {
        auto go = gameObject()->scene()->createGameObject("Label");
        go->transform()->setParent(transform());
        auto labelComponent = go->addComponent<Label>();

        registerComponent2D(labelComponent);

        auto font = Project::loadFont(fontsize);
        labelComponent->setFont(font);
        labelComponent->setText(text);

        return labelComponent;
    }

    void Canvas::updateRenderOrder(std::shared_ptr<Component2D> comp) {
        // todo
    }


    void Canvas::update() {
        if (mCamera ==nullptr || !enabled()) {
            return;
        }
        vec2 mousePosition = (vec2) MouseInput::position();

        vec2 screensize = (vec2) Engine::context()->getContextSurfaceDim();
        vec2 mouseClipCoord = ((mousePosition / screensize)*2.0f-vec2{1.0})*vec2{1,-1}; // correct

        mat4 viewProjection = inverse(mCamera->projectionMatrix() * mCamera->viewMatrix());
        vec2 mouseWorldCoord = (vec2)(viewProjection * vec4(mouseClipCoord, 0, 1));

        for (auto ml : mMouseListeners){
            auto sprite = dynamic_pointer_cast<Sprite>(ml);
            Bounds2 bounds = sprite->trimmedBounds();
            vec2 mouseLocalCoord = (vec2)(sprite->transform()->globalTRSInverse() * vec4{mouseWorldCoord, 0, 1});

            auto mouseOverIter = find(mMouseOver.begin(), mMouseOver.end(), ml);
            bool wasMouseOver = mouseOverIter != mMouseOver.end();
            if (bounds.contains(mouseLocalCoord)){
                if (!wasMouseOver){
                    ml->over();
                    mMouseOver.push_back(ml);
                }
                for (int i=0;i<3;i++){
                    if (MouseInput::down(i)){
                        ml->down(i);
                        mMousePressed.push_back({ml, i});
                    }
                }

            } else {
                if (wasMouseOver){
                    ml->out();
                    mMouseOver.erase(mouseOverIter);
                }
            }
        }
        for (int i = mMousePressed.size()-1;i>=0;i--){
            int button = mMousePressed[i].second;
            if (MouseInput::pressed(button)){
                mMousePressed[i].first->pressed(button);
            } else {
                mMousePressed[i].first->up(button);
                mMousePressed.erase(mMousePressed.begin()+i);
            }
        }
    }

    std::shared_ptr<ToggleButton> Canvas::createToggleButton(std::string text) {
        std::shared_ptr<TextureAtlas> textureAtlas = Project::loadTextureAtlas("assets/ui/ui.txt");
        GameObject *gameObject_ = gameObject()->scene()->createGameObject("Button");
        gameObject_->transform()->setParent(transform());
        auto button = gameObject_->addComponent<ToggleButton>();
        registerComponent2D(button);
        button->setText("Button");
        button->setTextureAtlas(textureAtlas);
        button->setNormalSprite("button-normal.png");
        button->setHoverSprite("button-hover.png");
        button->setPressedSprite("button-pressed.png");
        button->setSelectedSprite("button-pressed.png");
        button->setScale({2,2});
        button->setText(text);

        return button;
    }
}