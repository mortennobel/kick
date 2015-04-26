//
// Created by morten on 26/07/14.
//


#pragma once

#include "kick/scene/component_renderable.h"
#include "kick/scene/updatable.h"
#include "kick/scene/camera.h"
#include "kick/2d/component2d.h"
#include "kick/texture/texture_atlas.h"
#include "kick/core/mouse_input.h"
#include <vector>

namespace kick {
    class Camera;
    class Button;
    class ToggleButton;
    class Sprite;
    class Label;
    class Mesh;
    class MeshData;
    class SpriteMouseListener;


    class Canvas : public ComponentRenderable, public Updatable {

    public:
        Canvas(GameObject *gameObject);
        virtual ~Canvas();

        virtual void deactivated() override;

        virtual void render(EngineUniforms *engineUniforms, Material* replacementMaterial = nullptr) override;


        virtual void update() override;

        virtual int renderOrder();

        Camera *camera() const;
        void setCamera(Camera *camera);

        //
        ToggleButton* createToggleButton();

        //
        Button* createButton();
        // helper function, which creates a gameobject and attaches an sprite object
        Sprite* createSprite(std::shared_ptr<TextureAtlas> textureAtlas, std::string spriteName, glm::vec2 pos = glm::vec2{0});
        //
        Label *createLabel(std::string text, int fontsize = 16);



    private:
        void updateVertexBuffer(std::vector<Sprite *> &sprites);
        void renderSprites(std::vector<Sprite*> &sprites, kick::EngineUniforms *engineUniforms, Material* replacementMaterial);
        friend class Component2D;
        void updateRenderOrder(Component2D* comp);
        void registerComponent2D(Component2D* comp);
        void deregisterComponent2D(Component2D* comp);
        Camera*mCamera = nullptr;
        std::vector<Component2D*> mComponents;
        std::vector<SpriteMouseListener*> mMouseListeners;
        std::vector<SpriteMouseListener*> mMouseOver;
        std::vector<std::pair<SpriteMouseListener*, int>> mMousePressed;
        Mesh *mMesh = nullptr;
        std::shared_ptr<MeshData> mMeshData;
        Material*mMaterial = nullptr;
    };
}
