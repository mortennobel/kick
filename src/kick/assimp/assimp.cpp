//
//  assimp.cpp
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 12/25/14.
//  Copyright (c) 2014 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/assimp/assimp.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <assimp/material.h>     // Post processing flags
#include "kick/scene/light.h"
#include "kick/core/debug.h"
#include "kick/mesh/mesh.h"
#include "kick/scene/mesh_renderer.h"
#include <map>
#include <fstream>

#ifndef NO_ASSIMP

namespace kick {
    namespace {

        glm::vec3 toVec3(aiVector3D v){
            return glm::vec3{v.x,v.y,v.z};
        }

        glm::vec3 toVec3(aiColor3D v){
            return glm::vec3{v.r,v.g,v.b};
        }

        void importNode(aiNode * node, Scene *scenePtr, Transform* parent, std::map<aiNode *,GameObject*>& nodeMap){
            for (int i=0;i<node->mNumChildren;i++){
                aiNode * childNode = node->mChildren[i];
                GameObject* go = scenePtr->createGameObject(childNode->mName.C_Str());
                go->transform()->setParent(parent);
                nodeMap[childNode] = go;

                aiMatrix4x4 aiMat4 = childNode->mTransformation;
                aiVector3D scaling;
                aiQuaternion rotation;
                aiVector3D position;
                aiMat4.Decompose(scaling, rotation, position);

                go->transform()->setLocalRotation(glm::quat(rotation.w,rotation.x,rotation.y,rotation.z));
                go->transform()->setLocalPosition(toVec3(position));
                go->transform()->setLocalScale(toVec3(scaling));

                importNode(childNode, scenePtr, go->transform(), nodeMap);
            }
        }

        void assignMesh(aiNode * node, int meshIndex, std::shared_ptr<Mesh> mesh, Material * pMaterial, std::map<aiNode *,GameObject*>& nodeMap) {
            for (int i=0;i<node->mNumChildren;i++){
                aiNode *child = node->mChildren[i];
                for (int j=0;j<child->mNumMeshes;j++){
                    if (child->mMeshes[j] == meshIndex){
                        GameObject *go = nodeMap[child];
                        MeshRenderer *mr = go->addComponent<MeshRenderer>();
                        mr->setMaterial(pMaterial);
                        mr->setMesh(mesh);
                    }
                }

                assignMesh(child, meshIndex, mesh, pMaterial, nodeMap);
            }
        }

        void doSceneProcessing(const aiScene* scene, Scene *scenePtr, AssImpSettings importSettings){
            std::map<aiNode *,GameObject*> nodeMap;
            std::vector<std::shared_ptr<Texture2D>> allTextures;
            // import nodes
            importNode(scene->mRootNode, scenePtr, nullptr, nodeMap);

            if (importSettings.cameras){
                for (int i=0;i<scene->mNumCameras;i++){
                    auto camera = scene->mCameras[i];
                    auto cameraNode = scene->mRootNode->FindNode(camera->mName);
                    if (cameraNode){
                        GameObject* cameraGO = nodeMap[cameraNode];
                        CameraPerspective* perspective = cameraGO->addComponent<CameraPerspective>();

                        float aspect = camera->mAspect;
                        float fovH = camera->mHorizontalFOV;
                        // todo
                        perspective->setFar(camera->mClipPlaneFar);
                        perspective->setNear(camera->mClipPlaneNear);
                    } else {
                        logWarning(std::string("Could not find camera ")+camera->mName.C_Str());
                    }
                }
            }
            if (importSettings.textures){
                for (int i=0;i<scene->mNumTextures;i++){
                    aiTexture* texture = scene->mTextures[i];
                    bool compressed = texture->mHeight == 0;
                    std::shared_ptr<Texture2D> tex;
                    if (compressed){
                        tex = Project::loadTexture2DFromMemory((char *) texture->pcData, texture->mWidth);
                    } else {
                        tex = std::make_shared<Texture2D>();
                        tex->setData(texture->mWidth, texture->mHeight, (char *) texture->pcData);
                    }
                    allTextures.push_back(tex);
                }
            }
            if (importSettings.lights){
                glm::vec3 ambientLight{0};
                for (int i=0;i<scene->mNumLights;i++){

                    aiLight* light = scene->mLights[i];
                    ambientLight += glm::vec3(toVec3(light->mColorAmbient));
                    auto lightNode = scene->mRootNode->FindNode(light->mName);
                    if (lightNode){
                        GameObject *lightGO = nodeMap[lightNode];
                        Light* lightComponent = lightGO->addComponent<Light>();
                        lightComponent->setColor(toVec3(light->mColorDiffuse));
                        lightComponent->setAttenuation(glm::vec3(light->mAttenuationConstant, light->mAttenuationLinear, light->mAttenuationQuadratic));
                        light->mPosition; // todo
                        light->mDirection; // todo

                        switch (light->mType){
                            case aiLightSource_DIRECTIONAL:
                                lightComponent->setLightType(LightType::Directional);
                                break;
                            case aiLightSource_POINT:
                                lightComponent->setLightType(LightType::Point);
                                break;
                            case aiLightSource_SPOT:
                                lightComponent->setLightType(LightType::Spot);
                                break;
                            case aiLightSource_UNDEFINED:
                                continue;
                        }
                    } else {
                        logWarning(std::string("Could not find light ")+light->mName.C_Str());
                    }
                }
                ambientLight *= 1.0f/scene->mNumLights;
                scenePtr->createAmbientLight(1.0, ambientLight);
            }
            std::vector<Material*> mats;
            if (importSettings.materials){
                for (int i=0;i<scene->mNumMaterials;i++){
                    aiMaterial* aiMaterial = scene->mMaterials[i];
                    Material *material = new Material();

                    float opacity = 1.0f;
                    aiMaterial->Get(AI_MATKEY_OPACITY, opacity);
                    float shininess = 0.0f;
                    aiMaterial->Get(AI_MATKEY_SHININESS, shininess);


                    aiColor3D color (0.f,0.f,0.f);
                    if (aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color)){
                        material->setUniform("mainColor", glm::vec4(toVec3(color), opacity));
                    }
                    int blend = 0;
                    aiMaterial->Get(AI_MATKEY_BLEND_FUNC,blend);

                    unsigned int numTextures = aiMaterial->GetTextureCount(aiTextureType_DIFFUSE);
                    aiString* texturePath = nullptr;
                    aiTextureMapping* mapping = nullptr;
                    unsigned int* uvindex = nullptr;
                    float* texBlend = nullptr;
                    aiTextureOp* op = nullptr;
                    aiTextureMapMode* mapmode = nullptr;

                    if (aiMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0 &&
                            aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, texturePath, mapping, uvindex, texBlend, op, mapmode) == AI_SUCCESS)
                    {
                        std::fstream file(texturePath->C_Str());
                        if (file.is_open()){
                            file.close();
                            auto tex = Project::loadTexture2D(texturePath->C_Str());
                            if (tex){
                                material->setUniform("mainTexture", tex);
                            }
                        } else {
                            int textureIndex = atoi(texturePath->C_Str());
                            material->setUniform("mainTexture", allTextures[textureIndex]);
                        }
                    }

                    if (blend){
                        if (shininess>0.0f) {
                            material->setShader(Project::loadShader("assets/shaders/transparent_specular.shader"));
                        } else {
                            material->setShader(Project::loadShader("assets/shaders/transparent_diffuse.shader"));
                        }
                    } else {
                        if (shininess>0.0f) {
                            material->setShader(Project::loadShader("assets/shaders/specular.shader"));
                        } else {
                            material->setShader(Project::loadShader("assets/shaders/diffuse.shader"));
                        }
                    }

                    mats.push_back(material);
                }
            }
            if (importSettings.meshes){
                for (int i=0;i<scene->mNumMeshes;i++){
                    aiMesh* aiMesh = scene->mMeshes[i];
                    std::shared_ptr<MeshData> meshData = std::make_shared<MeshData>();
                    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
                    mesh->setMeshData(meshData);
                    assignMesh(scene->mRootNode, i, mesh, mats[aiMesh->mMaterialIndex], nodeMap);
                }
            }
        }
    }

    bool AssImp::importData(std::string filename, Scene *scenePtr, AssImpSettings importSettings) {
        // Create an instance of the Importer class
        Assimp::Importer importer;
        // And have it read the given file with some example postprocessing
        // Usually - if speed is not the most important aspect for you - you'll
        // propably to request more postprocessing than we do in this example.
        const aiScene* scene = importer.ReadFile( filename,
                aiProcess_CalcTangentSpace       |
                        aiProcess_Triangulate            |
                        aiProcess_JoinIdenticalVertices  |
                        aiProcess_SortByPType);

        // If the import failed, report it
        if( !scene)
        {
            logError(importer.GetErrorString());
            return false;
        }
        // Now we can access the file's contents.
        doSceneProcessing(scene, scenePtr, importSettings);

        // We're done. Everything will be cleaned up by the importer destructor
        return true;
    }
}

#endif
