//
// Created by morten on 26/07/14.
//

#include "panel.h"
#include "kick/2d/text.h"
#include "kick/2d/sprite.h"
#include "kick/2d/button.h"
#include "kick/scene/scene.h"
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

    Panel::Panel(GameObject *gameObject) : ComponentRenderable(gameObject) {
        mesh = Project::createAsset<Mesh>();
        meshData = Project::createAsset<MeshData>();
        meshData->setMeshUsage(MeshUsage::DynamicDraw);
        mesh->setMeshData(meshData);
        material = Project::createAsset<Material>();
        mouseInput = &Engine::instance->getMouseInput();
    }

    Panel::~Panel() {

    }

    void Panel::render(kick::EngineUniforms *engineUniforms) {
        sort(components.begin(), components.end(), [](Component2D *c1, Component2D *c2){
            int order = c1->getOrder() - c2->getOrder();
            return order < 0 || (order==0 &&
                    (c1->getShader() < c2->getShader()));
        });

        vector<Sprite*> sprites;
        TextureAtlas* textureAtlas = nullptr;
        for (auto iter = components.begin();iter != components.end(); iter++){
            auto text = dynamic_cast<Text*>(*iter);
            if (text){
                renderSprites(sprites, engineUniforms);
                text->render(engineUniforms);
            } else {
                auto sprite = dynamic_cast<Sprite*>(*iter);
                if (textureAtlas != sprite->getTextureAtlas().get()){
                    renderSprites(sprites, engineUniforms);
                    textureAtlas = sprite->getTextureAtlas().get();
                }
                if (sprite){
                    sprites.push_back(sprite);
                }
            }
        }
        renderSprites(sprites, engineUniforms);
    }

    void Panel::updateVertexBuffer(std::vector<Sprite *> &sprites) {
        vector<vec3> position;
        vector<vec2> textureCoords;
        vector<vec4> colors;
        vector<GLushort> indices;
        sort(sprites.begin(), sprites.end(), [](Sprite* s1, Sprite* s2){
            return s1->getOrder() < s2->getOrder();
        });
        unsigned short index = 0;
        for (unsigned short i=0;i<sprites.size();i++){
            Sprite * sprite = sprites[i];
            Transform* transform = sprite->getTransform();

            vec2 size = (vec2)sprite->getTextureAtlas()->getTextureSize();
            mat4 toWorld = transform->getGlobalMatrix();
            vec2 scale = sprite->getScale();


            TextureAtlasEntry entry = sprite->getEntry();
            Bounds2D bounds = sprite->getTrimmedBounds();
            vec4 color = sprite->getColor();

            if (sprite->getType() == SpriteType::Simple) {
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
            else if (sprite->getType() == SpriteType::Sliced) {
                vec2 dim {entry.frame.z,entry.frame.w};
                vec4 sliceX = vec4{bounds.min.x, bounds.min.x + dim.x * sprite->getSliceX()[0],bounds.max.x - dim.x * (1.0-sprite->getSliceX()[1]), bounds.max.x};
                vec4 sliceY = vec4{bounds.min.y, bounds.min.y + dim.y * sprite->getSliceY()[0],bounds.max.y - dim.y * (1.0-sprite->getSliceY()[1]), bounds.max.y};

                vec2 min{entry.frame.x / size.x, 1.0 - (entry.frame.y + entry.frame.w) / size.y};
                vec2 max{(entry.frame.x + entry.frame.z) / size.x, 1.0 - entry.frame.y / size.y};
                vec4 uvX{min.x, lerp(min.x, max.x, sprite->getSliceX()[0]),lerp(min.x, max.x, sprite->getSliceX()[1]),max.x};
                vec4 uvY{min.y, lerp(min.y, max.y, sprite->getSliceY()[0]),lerp(min.y, max.y, sprite->getSliceY()[1]),max.y};

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
        meshData->setPosition(position);
        meshData->setColor(colors);
        meshData->setTexCoord0(textureCoords);
        meshData->setSubmesh(0,indices, MeshType::Triangles);
        mesh->setMeshData(meshData);
    }

    void Panel::renderSprites(std::vector<Sprite *> &sprites, kick::EngineUniforms *engineUniforms) {
        if (sprites.size()==0){
            return;
        }
        updateVertexBuffer(sprites);

        material->setShader(sprites[0]->getTextureAtlas()->getShader());
        auto shader = material->getShader();
        assert(shader);
        mesh->bind(shader.get());

        material->setUniform("mainTexture", sprites[0]->getTextureAtlas()->getTexture());
        shader->bind_uniforms(material, engineUniforms, getTransform());

        mesh->render(0);

        sprites.clear();
    }

    int Panel::getRenderOrder() {
        return 0;
    }

    Camera *Panel::getCamera() const {
        return camera;
    }

    void Panel::setCamera(Camera *camera) {
        Panel::camera = camera;
        int cullingMask = camera->getCullingMask();
        gameObject->setLayer((int)round(log2((float)cullingMask))+1); // set to largest value
    }

    void Panel::activated() {
        for (auto c : gameObject->getComponentsInChildren<Component2D>()){
            registerComponent2D(c);
        }
    }

    void Panel::deactivated() {
        for (auto c : components){
            deregisterComponent2D(c);
        }
    }

    void Panel::registerComponent2D(Component2D *comp) {
        components.push_back(comp);
        comp->panel = this;

        SpriteMouseListener *sml = dynamic_cast<SpriteMouseListener*>(comp);
        if (sml){
            mouseListeners.push_back(sml);
        }
    }

    void Panel::deregisterComponent2D(Component2D *comp) {
        auto pos = find(components.begin(), components.end(), comp);
        if (pos != components.end()){
            (*pos)->panel = nullptr;
            components.erase(pos);

            SpriteMouseListener *sml = dynamic_cast<SpriteMouseListener*>(comp);
            auto pos2 = find(mouseListeners.begin(), mouseListeners.end(), sml);
            if (pos2 != mouseListeners.end()){
                mouseListeners.erase(pos2);
            }
        }
    }

    Sprite *Panel::createSprite(std::shared_ptr<TextureAtlas> textureAtlas, std::string spriteName, glm::vec2 pos) {

        GameObject *gameObject = getGameObject()->getScene()->createGameObject("Sprite");
        gameObject->getTransform()->setParent(getTransform());
        Sprite* sprite = gameObject->addComponent<Sprite>();
        sprite->setTextureAtlas(textureAtlas);
        sprite->setSpriteName(spriteName);
        return sprite;
    }

    Button *Panel::createButton() {
        std::shared_ptr<TextureAtlas> textureAtlas = Project::loadTextureAtlas("assets/ui/ui.txt", "assets/ui/ui.png");
        GameObject *gameObject = getGameObject()->getScene()->createGameObject("Button");
        gameObject->getTransform()->setParent(getTransform());
        Button* button = gameObject->addComponent<Button>();
        button->setText("Button");
        button->setTextureAtlas(textureAtlas);
        button->setNormal("button-normal.png");
        button->setHover("button-hover.png");
        button->setPressed("button-pressed.png");
        button->setScale({2,2});

        return button;
    }

    Text *Panel::createText(std::string text) {
        auto go = getGameObject()->getScene()->createGameObject("Font");
        go->getTransform()->setParent(getTransform());
        Text* textComponent = go->addComponent<Text>();

        Font* font = Project::createAsset<Font>();
        textComponent->setFont(font);
        textComponent->setText(text);

        return textComponent;
    }

    void Panel::updateRenderOrder(Component2D *comp) {
        // todo
    }


    void Panel::update() {
        if (camera==nullptr) {
            return;
        }
        vec2 mousePosition = (vec2)mouseInput->getPosition();
        vec2 screensize = (vec2)Engine::instance->getContext()->getContextSurfaceDim();
        vec2 mouseClipCoord = ((mousePosition / screensize)*2.0f-vec2{1.0})*vec2{1,-1}; // correct

        mat4 viewProjection = inverse(camera->getProjectionMatrix() * camera->getViewMatrix());
        vec2 mouseWorldCoord = (vec2)(viewProjection * vec4(mouseClipCoord, 0, 1));
        int index = 0;
        for (auto ml : mouseListeners){
            Sprite* sprite = dynamic_cast<Sprite*>(ml);
            Bounds2D bounds = sprite->getTrimmedBounds();
            vec2 mouseLocalCoord = (vec2)(sprite->getTransform()->getGlobalTRSInverse() * vec4{mouseWorldCoord, 0, 1});

            auto mouseOverIter = find(mouseOver.begin(), mouseOver.end(), ml);
            bool wasMouseOver = mouseOverIter != mouseOver.end();
            if (bounds.contains(mouseLocalCoord)){
                if (!wasMouseOver){
                    ml->over();
                    mouseOver.push_back(ml);
                }
                for (int i=0;i<3;i++){
                    if (mouseInput->down(i)){
                        ml->down(i);
                        mousePressed.push_back({ml, i});
                    }
                }

            } else {
                if (wasMouseOver){
                    ml->out();
                    mouseOver.erase(mouseOverIter);
                }
            }
        }
        for (int i=mousePressed.size()-1;i>=0;i--){
            int button = mousePressed[i].second;
            if (mouseInput->pressed(button)){
                mousePressed[i].first->pressed(button);
            } else {
                mousePressed[i].first->up(button);
                mousePressed.erase(mousePressed.begin()+i);
            }
        }
    }
}