//
// Created by morten on 30/06/14.
//

#include "kick/sprite/spritemanager.h"
#include "kick/sprite/sprite.h"
#include "kick/core/log.h"
#include "font.h"
#include "transform.h"
#include <vector>

using namespace std;
using namespace glm;

namespace kick {
    SpriteManager::SpriteManager(GameObject *gameObject) : ComponentRenderable(gameObject) {
        mesh = Project::createAsset<Mesh>();
        meshData = Project::createAsset<MeshData>();
        mesh->setMeshData(meshData);
        material = Project::createAsset<Material>();
        material->setShader(Project::loadShader("assets/shaders/transparent_unlit.shader"));
    }

    void SpriteManager::render(EngineUniforms *engineUniforms) {
        updateVertexBuffer();
        auto shader = material->getShader();
        assert(shader);
        mesh->bind(shader);
        if (meshData->getSubmeshesCount()){
            material->setUniform("mainTexture", sprites[0]->getTextureAtlas()->getTexture());
            shader->bind_uniforms(material, engineUniforms, getTransform());

            mesh->render(0);
        }
    }

    SpriteManager *SpriteManager::getInstance(Scene *scene) {
        SpriteManager * lastUsed = nullptr;
        if (lastUsed && lastUsed->scene == scene){
            return lastUsed;
        }
        // todo search scene for instance
        auto go = scene->createGameObject("SpriteManager");
        lastUsed = go->addComponent<SpriteManager>();
        return lastUsed;
    }

    void SpriteManager::updateVertexBuffer() {
        vector<vec3> position;
        vector<vec2> textureCoords;
        vector<GLushort> indices;
        for (unsigned short i=0;i<sprites.size();i++){
            Sprite * sprite = sprites[i];
            Transform* transform = sprite->getTransform();
            TextureAtlasEntry entry = sprite->getEntry();
            vec2 size = (vec2)sprite->getTextureAtlas()->getTextureSize();
            mat4 toWorld = transform->getGlobalMatrix();

            vec2 basePoint = vec2{0,0};
            float w = entry.frame.z;
            float h = entry.frame.w;
            position.push_back((vec3)(toWorld * vec4{basePoint,0,1}));
            position.push_back((vec3)(toWorld * vec4{basePoint+vec2{w,0},0,1}));
            position.push_back((vec3)(toWorld * vec4{basePoint+vec2{w,h},0,1}));
            position.push_back((vec3)(toWorld * vec4{basePoint+vec2{0,h},0,1}));

            vec2 min{entry.frame.x / size.x,1.0 - (entry.frame.y+entry.frame.w) / size.y};
            vec2 max{(entry.frame.x+entry.frame.z) / size.x,1.0 - entry.frame.y / size.y};
            textureCoords.push_back(vec2{min.x,min.y});
            textureCoords.push_back(vec2{max.x,min.y});
            textureCoords.push_back(vec2{max.x,max.y});
            textureCoords.push_back(vec2{min.x,max.y});

            // push two triangles
            indices.push_back(i*4);
            indices.push_back(i*4+1);
            indices.push_back(i*4+2);
            indices.push_back(i*4+2);
            indices.push_back(i*4+3);
            indices.push_back(i*4);
        }
        meshData->setPosition(position);
        meshData->setTexCoord0(textureCoords);
        meshData->setSubmesh(0,indices, MeshType::Triangles);
        mesh->setMeshData(meshData);
    }

    void SpriteManager::registerSprite(Sprite *sprite) {
        sprites.push_back(sprite);
//        auto tex = sprite->getTextureAtlas()->getTexture();
//        auto spritesForTex = spritesGrouped.find(tex);
//        if (spritesForTex == spritesGrouped.end()){
//            spritesGrouped[tex] = {};
//            spritesForTex = spritesGrouped.find(tex);
//        }
//        spritesForTex->second.push_back(sprite);
    }

    void SpriteManager::deregisterSprite(Sprite *sprite) {
        auto pos = find(sprites.begin(),sprites.end(), sprite);
        if (pos != sprites.end()){
            sprites.erase(pos);
        }

        // todo improve this
//        for (auto i : spritesGrouped){
//            auto pos = find(i.second.begin(),i.second.end(), sprite);
//            if (pos != i.second.end()){
//                i.second.erase(pos);
//            }
//        }
    }

    Shader *SpriteManager::getShader() const {
        return material->getShader();
    }

    void SpriteManager::setShader(Shader *shader) {
        material->setShader(shader);
    }
}