//
// Created by morten on 30/06/14.
//

#pragma once

#include "kick/scene/component_renderable.h"
#include "kick/scene/game_object.h"
#include "kick/scene/scene.h"
#include "kick/mesh/mesh.h"
#include "kick/mesh/mesh_data.h"
#include "kick/material/material.h"
#include <vector>

namespace kick {
    class Texture2D;

class SpriteManager : public ComponentRenderable {
public:
    SpriteManager(GameObject *gameObject);

    virtual void render(EngineUniforms *engineUniforms) override;

    static SpriteManager *getInstance(Scene * scene);

    Shader * getShader() const;
    void setShader(Shader *shader);
private:
    void updateVertexBuffer();
    friend class Sprite;
    void registerSprite(Sprite* sprite);
    void deregisterSprite(Sprite* sprite);
    Scene* scene;
    std::vector<Sprite*> sprites;

    Mesh *mesh = nullptr;
    MeshData *meshData = nullptr;
    Material* material = nullptr;
};
}



