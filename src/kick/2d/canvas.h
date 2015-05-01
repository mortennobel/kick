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

        std::shared_ptr<Camera> camera() const;
        void setCamera(std::shared_ptr<Camera> camera);

        template <typename C, typename... T>
        std::shared_ptr<C> addComponent(T... t);

        //
        std::shared_ptr<ToggleButton> createToggleButton(std::string text = "");

        //
        std::shared_ptr<Button> createButton(std::string text = "");
        // helper function, which creates a gameobject and attaches an sprite object
        std::shared_ptr<Sprite> createSprite(std::shared_ptr<TextureAtlas> textureAtlas, std::string spriteName, glm::vec2 pos = glm::vec2{0});
        //
        std::shared_ptr<Label> createLabel(std::string text, int fontsize = 16);

    private:
        void updateVertexBuffer(std::vector<Sprite*> &sprites);
        void renderSprites(std::vector<Sprite*> &sprites, kick::EngineUniforms *engineUniforms, Material* replacementMaterial);
        friend class Component2D;
        void updateRenderOrder(std::shared_ptr<Component2D> comp);
        void registerComponent2D(std::shared_ptr<Component2D> comp);
        void deregisterComponent2D(std::shared_ptr<Component2D> comp);
        std::shared_ptr<Camera> mCamera;
        std::shared_ptr<Camera> mOwnCamera;
        std::vector<std::shared_ptr<Component2D>> mComponents;
        std::vector<std::shared_ptr<SpriteMouseListener>> mMouseListeners;
        std::vector<std::shared_ptr<SpriteMouseListener>> mMouseOver;
        std::vector<std::pair<std::shared_ptr<SpriteMouseListener>, int>> mMousePressed;
        Mesh *mMesh = nullptr;
        std::shared_ptr<MeshData> mMeshData;
        Material*mMaterial = nullptr;
        GameObject *createGameObject();
    };
}

#include "kick/2d/canvas.inl"
