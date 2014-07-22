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
        meshData->setMeshUsage(MeshUsage::DynamicDraw);
        mesh->setMeshData(meshData);
        material = Project::createAsset<Material>();
    }

    void SpriteManager::render(EngineUniforms *engineUniforms) {
        updateVertexBuffer();
        if (meshData->getSubmeshesCount()){
            material->setShader(sprites[0]->getTextureAtlas()->getShader());
            auto shader = material->getShader();
            assert(shader);
            mesh->bind(shader.get());

            material->setUniform("mainTexture", sprites[0]->getTextureAtlas()->getTexture());
            shader->bind_uniforms(material, engineUniforms, getTransform());

            mesh->render(0);
        }
    }

    SpriteManager *SpriteManager::getInstance(Scene *scene) {
        for (auto & spriteManager : scene->findComponents<SpriteManager>()){
            if (spriteManager->scene == scene){
                return spriteManager;
            }
        }
        auto go = scene->createGameObject("SpriteManager");
        SpriteManager * lastUsed = go->addComponent<SpriteManager>();
        lastUsed->scene = scene;
        return lastUsed;
    }

    void SpriteManager::updateVertexBuffer() {
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

    void SpriteManager::registerSprite(Sprite *sprite) {
        sprites.push_back(sprite);
    }

    void SpriteManager::deregisterSprite(Sprite *sprite) {
        auto pos = find(sprites.begin(),sprites.end(), sprite);
        if (pos != sprites.end()){
            sprites.erase(pos);
        }
    }

    std::shared_ptr<Shader> SpriteManager::getShader() const {
        return material->getShader();
    }

    void SpriteManager::setShader(std::shared_ptr<Shader> shader) {
        material->setShader(shader);
    }

    int SpriteManager::getRenderOrder() {
        if (material == nullptr){
            return 0;
        }
        return material->getRenderOrder();
    }
}