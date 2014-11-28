//
//  EngineTest.h
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 26/10/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once

#include "EngineTest.h"
#include "tinytest/tinytest.h"
#include "kick/kick.h"

#include "glm/gtx/string_cast.hpp"


using namespace kick;
using namespace std;
using namespace glm;

void initEngine(int& argc, char** argv){
    // Manually create engine instance
    //GLUTContext cont;

    Engine::init(argc, argv);
    kick::Log::disable();
}

int TestShader() {
    Shader *shader;

    shader = new Shader("foo", "foo");

    TINYTEST_ASSERT(shader->apply() == false);


    shader = new Shader();
    TINYTEST_ASSERT(shader->apply() == false);


    // mismatch between out and in parameters
    string vertexShader =
            R"(
            uniform vec2 p;
            in vec4 position;
            void main (void)
            {
                gl_Position = vec4(p, 0.0, 0.0) + position;
            }
            )";


    string fragmentShader =
            R"(
            in vec4 colorV;
            out vec4 fragColor;
            void main(void)
            {
                fragColor = colorV;
            }
            )";

    shader = new Shader(vertexShader, fragmentShader);

    TINYTEST_ASSERT_MSG(shader == nullptr || !shader->apply(), "Warn linker error not found.");

    vertexShader =
            R"(
            uniform vec2 p;
            in vec4 position;
            in vec4 color;
            out vec4 colorV;
            void main (void)
            {
                colorV = color;
                gl_Position = vec4(p.xyz, 0.0) + position;
            }
            )";


    fragmentShader =
            R"(
            in vec4 colorV;
            out vec4 fragColor;
            void main(void)
            {
                fragColor = colorV;
            }
            )";

    shader = new Shader(vertexShader, fragmentShader);
    TINYTEST_ASSERT_MSG(shader  == nullptr || !shader->apply(), "Vertex Shader Compiler error not found.");

    vertexShader =
            R"(
            uniform vec2 p;
            in vec4 position;
            in vec4 color;
            out vec4 colorV;
            void main (void)
            {
                colorV = color;
                gl_Position = vec4(p, 0.0, 0.0) + position;
            }
            )";


    fragmentShader =
            R"(
            in vec4 colorV;
            out vec4 fragColor;
            void main(void)
            {
                fragColor = colorV;
            }
            )";

    shader = new Shader(vertexShader, fragmentShader);
    shader->apply();

    auto shaderUniforms = shader->getShaderUniforms();
    TINYTEST_ASSERT(shaderUniforms.size()==1);
    TINYTEST_ASSERT(shaderUniforms[0].name.compare("p") == 0);
//    TINYTEST_ASSERT(shaderUniforms[0].index == 0);
    TINYTEST_ASSERT(shaderUniforms[0].size == 1);
    TINYTEST_ASSERT(shaderUniforms[0].type == GL_FLOAT_VEC2);

    auto shaderAttributes = shader->shaderAttributes();
    TINYTEST_ASSERT(shaderAttributes.size() == 2);
    for (auto shaderAttribute:shaderAttributes){
        TINYTEST_ASSERT(shaderAttribute.name.compare("position") == 0 || shaderAttribute.name.compare("color") == 0);
//        TINYTEST_ASSERT(shaderAttribute.index < 2);
        TINYTEST_ASSERT(shaderAttribute.size == 1);
        TINYTEST_ASSERT(shaderAttribute.type == GL_FLOAT_VEC4);
    }

    return 1;
}

int TestMaterial() {
    std::string vertexShader =
            R"(
            uniform vec4 p;
            in vec4 position;
            in vec4 color;
            out vec4 colorV;
            void main (void)
            {
                colorV = color;
                gl_Position = p + position;
            }
            )";


    std::string fragmentShader =
            R"(
            in vec4 colorV;
            out vec4 fragColor;
            void main(void)
            {
                fragColor = colorV;
            }
            )";
    auto shader = std::shared_ptr<Shader>{new Shader(vertexShader, fragmentShader)};

    shader->apply();
    Material* material = new Material();
    material->setShader(shader);
    material->setUniform("p", glm::vec4{0,1,0,1});

    return 1;
}

int TestMeshData(){
    MeshData* meshData = new MeshData();

    const vec3 p0{0,0,0};
    const vec3 p1{1,0,0};
    const vec3 p2{0.5f,0,std::sqrt(0.75f)};
    const vec3 p3{0.5f,std::sqrt(0.75f),std::sqrt(0.75f)/3};

    meshData->setPosition({
            p0,p1,p2,
            p0,p2,p3,
            p2,p1,p3,
            p0,p3,p1
    });
    meshData->recomputeBounds();

    meshData->setSubmesh(0, {
            0,1,2,
            3,4,5,
            6,7,8,
            9,10,11
    }, MeshType::Triangles);
    meshData->recomputeBounds();
    Bounds3 aabb = meshData->bounds();
    TINYTEST_ASSERT(aabb.min == p0);
    vec3 vec3max{1,std::sqrt(0.75f),std::sqrt(0.75f)};
    TINYTEST_ASSERT(aabb.max == vec3max);

    meshData->recomputeNormals();
    TINYTEST_ASSERT(meshData->normal().size() == meshData->position().size());
    for (auto normal : meshData->normal()){
        TINYTEST_ASSERT(normal != vec3(0));
    }

    auto interleavedData = meshData->interleavedData();
    auto interleavedFormat = meshData->interleavedFormat();
    TINYTEST_ASSERT(interleavedFormat.size() == 2);

    //if (interleavedFormat[0].)


    return 1;
}



int TestMesh() {
    //Mesh mesh;
    return 1;
}

int TestMeshFactory(){
    MeshFactory::createPointData();
    MeshFactory::createTriangleData();
    MeshFactory::createDiscData();
    MeshFactory::createPlaneData();
    MeshFactory::createUVSphereData();
    MeshFactory::createCubeData();

    return 1;
}

int TestLoadTextFile(){
    string test;
    bool res = Project::loadTextResource("unittest/testasset/txt.txt", test);
    TINYTEST_ASSERT(res);
    TINYTEST_ASSERT(test == "hello world");
    return 1;
}

int TestLoadBinaryFile(){
    vector<char> res;
    bool success = Project::loadBinaryResource("unittest/testasset/txt.txt", res);
    TINYTEST_ASSERT(success);
    string helloWorld = "hello world";
    TINYTEST_ASSERT(10 == res[res.size()-1]);
    for (int i=0;i<res.size()-1;i++){
        TINYTEST_ASSERT(helloWorld[i] == res[i]);
    }
    return 1;
}

int TestLoadTexture(){
    auto img = Project::loadTexture2D("assets/textures/logo.png");

    return 1;
}

int TestLoadTextureTypes(){
#ifndef EMSCRIPTEN
    GLenum error = glGetError();
    TINYTEST_ASSERT_MSG(error == GL_NO_ERROR, "Init");
    string imgPaths[6] = {
            "unittest/images/pnggrad8rgb.png",
            "unittest/images/pnggrad16rgb.png",
            "unittest/images/pngtest8rgba.png",
            "unittest/images/pngtest16rgba.png",
            "unittest/images/load_0_.png",
            "unittest/images/load_0.png"
    };

    int expectedTypes[6] = {GL_RGB,GL_RGB,GL_RGBA,GL_RGBA};
    for (int i=0;i<6;i++){
        auto img = Project::loadTexture2D(imgPaths[i]);
        error = glGetError();
        TINYTEST_ASSERT_MSG(error == GL_NO_ERROR, imgPaths[i].c_str());
    }
#endif
    return 1;

}

int TestDefaultShaders(){
    vector<string> shaders {
            "diffuse",
            "skybox",
            "specular",
            "transparent_diffuse",
            "transparent_point_sprite",
            "transparent_specular",
            "transparent_unlit",
            "unlit",
            "unlit_vertex_color",
            "font",
            "__error",
            "__pick",
            "__pick_normal",
            "__pick_uv",
            "__shadowmap",
    };

    int errors = 0;
    for (auto & s : shaders){
        string vertexShaderURI = string{"assets/shaders/"}+s+"_vs.glsl";
        string fragmentShaderURI = string{"assets/shaders/"}+s+"_fs.glsl";
        string vertexShader;
        bool success = Project::loadTextResource(vertexShaderURI, vertexShader);
        TINYTEST_ASSERT(success);
        string fragmentShader;
        success = Project::loadTextResource(fragmentShaderURI, fragmentShader);
        TINYTEST_ASSERT(success);

        Shader* shader = new Shader(vertexShader, fragmentShader);

        success = shader->apply();
        if (!success){
            errors++;
        }

        GLenum error = glGetError();
        TINYTEST_ASSERT_MSG(error == GL_NO_ERROR, s.c_str());
    }
    TINYTEST_ASSERT(errors == 0);
    return 1;
}

int TestShaderPreCompiler(){
    string removeCommentedOutPragmas = R"(//#pragma include "light.glsl"
test
//#pragma include "light2.glsl"
#pragma include "assets/shaders/light.glsl")";
    string output = Shader::getPrecompiledSource(removeCommentedOutPragmas, ShaderType::FragmentShader);

    TINYTEST_ASSERT(output.find("//#pragma") == -1);
    return 1;
}

int TestShaderLoading(){
    vector<string> shaders {
            "diffuse",
            "skybox",
            "specular",
            "transparent_diffuse",
            "transparent_point_sprite",
            "transparent_specular",
            "transparent_unlit",
            "unlit",
            "unlit_vertex_color",
            "__error",
            "__pick",
            "__pick_normal",
            "__pick_uv",
            "__shadowmap",
    };
    for (auto & s : shaders){
        string shaderURI = string{"assets/shaders/"} + s + ".shader";
        auto shader = Project::loadShader(shaderURI);
        TINYTEST_ASSERT(shader != nullptr);
        TINYTEST_ASSERT(shader->getShaderSource(kick::ShaderType::VertexShader).length() > 0);
        TINYTEST_ASSERT(shader->getShaderSource(kick::ShaderType::FragmentShader).length() > 0);
        bool isTransparent = s.substr(0, 5) == "trans";
        TINYTEST_ASSERT(isTransparent == shader->blend());

    }
    return 1;
}

int TestKeyInputButton(){

    KeyInput::reset();
    TINYTEST_EQUAL(false, KeyInput::pressed(Key::x));
    TINYTEST_EQUAL(false, KeyInput::down(Key::x));
    TINYTEST_EQUAL(false, KeyInput::up(Key::x));

    KeyInput::pressBegin(Key::x);
    TINYTEST_EQUAL(true, KeyInput::pressed(Key::x));
    TINYTEST_EQUAL(true, KeyInput::down(Key::x));
    TINYTEST_EQUAL(false, KeyInput::up(Key::x));

    KeyInput::reset();
    TINYTEST_EQUAL(true, KeyInput::pressed(Key::x));
    TINYTEST_EQUAL(false, KeyInput::down(Key::x));
    TINYTEST_EQUAL(false, KeyInput::up(Key::x));

    KeyInput::pressEnd(Key::x);
    TINYTEST_EQUAL(false, KeyInput::pressed(Key::x));
    TINYTEST_EQUAL(false, KeyInput::down(Key::x));
    TINYTEST_EQUAL(true, KeyInput::up(Key::x));

    KeyInput::reset();
    TINYTEST_EQUAL(false, KeyInput::pressed(Key::x));
    TINYTEST_EQUAL(false, KeyInput::down(Key::x));
    TINYTEST_EQUAL(false, KeyInput::up(Key::x));

    return 1;
}

int TestKeyMapping(){
    KeyMapping keyMappingX{Key::x};

    KeyInput::reset();
    TINYTEST_EQUAL(false, KeyInput::pressed(keyMappingX));
    TINYTEST_EQUAL(false, KeyInput::down(keyMappingX));

    KeyInput::pressBegin(Key::x);
    TINYTEST_EQUAL(true, KeyInput::pressed(keyMappingX));
    TINYTEST_EQUAL(true, KeyInput::down(keyMappingX));

    KeyInput::reset();
    TINYTEST_EQUAL(true, KeyInput::pressed(keyMappingX));
    TINYTEST_EQUAL(false, KeyInput::down(keyMappingX));

    KeyInput::pressEnd(Key::x);
    TINYTEST_EQUAL(false, KeyInput::pressed(keyMappingX));
    TINYTEST_EQUAL(false, KeyInput::down(keyMappingX));

    KeyInput::reset();
    TINYTEST_EQUAL(false, KeyInput::pressed(keyMappingX));
    TINYTEST_EQUAL(false, KeyInput::down(keyMappingX));

    return 1;
}

int TestKeyMappingMulti(){
    KeyMapping keyMappingXY{Key::x, Key::y};

    KeyInput::reset();
    TINYTEST_EQUAL(false, KeyInput::pressed(keyMappingXY));
    TINYTEST_EQUAL(false, KeyInput::down(keyMappingXY));

    KeyInput::pressBegin(Key::x);
    TINYTEST_EQUAL(false, KeyInput::pressed(keyMappingXY));
    TINYTEST_EQUAL(false, KeyInput::down(keyMappingXY));

    KeyInput::reset();
    TINYTEST_EQUAL(false, KeyInput::pressed(keyMappingXY));
    TINYTEST_EQUAL(false, KeyInput::down(keyMappingXY));

    KeyInput::reset();

    KeyInput::pressBegin(Key::y);
    TINYTEST_EQUAL(true, KeyInput::pressed(keyMappingXY));
    TINYTEST_EQUAL(true, KeyInput::down(keyMappingXY));

    KeyInput::reset();
    TINYTEST_EQUAL(true, KeyInput::pressed(keyMappingXY));
    TINYTEST_EQUAL(false, KeyInput::down(keyMappingXY));

    KeyInput::pressEnd(Key::x);
    TINYTEST_EQUAL(false, KeyInput::pressed(keyMappingXY));
    TINYTEST_EQUAL(false, KeyInput::down(keyMappingXY));

    KeyInput::reset();
    TINYTEST_EQUAL(false, KeyInput::pressed(keyMappingXY));
    TINYTEST_EQUAL(false, KeyInput::down(keyMappingXY));

    return 1;
}


int TestMouseInputButton(){

    TINYTEST_EQUAL(false, MouseInput::pressed(0));
    TINYTEST_EQUAL(false, MouseInput::down(1));
    TINYTEST_EQUAL(false, MouseInput::up(2));
    TINYTEST_EQUAL(false, MouseInput::up(3));
    TINYTEST_EQUAL(false, MouseInput::up(4));

    MouseInput::buttonPressStarted(0);
    TINYTEST_EQUAL(true, MouseInput::pressed(0));
    TINYTEST_EQUAL(true, MouseInput::down(0));
    TINYTEST_EQUAL(false, MouseInput::up(0));

    MouseInput::reset();
    TINYTEST_EQUAL(true, MouseInput::pressed(0));
    TINYTEST_EQUAL(false, MouseInput::down(0));
    TINYTEST_EQUAL(false, MouseInput::up(0));

    MouseInput::buttonPressEnded(0);
    TINYTEST_EQUAL(false, MouseInput::pressed(0));
    TINYTEST_EQUAL(false, MouseInput::down(0));
    TINYTEST_EQUAL(true, MouseInput::up(0));

    MouseInput::reset();
    TINYTEST_EQUAL(false, MouseInput::pressed(0));
    TINYTEST_EQUAL(false, MouseInput::down(0));
    TINYTEST_EQUAL(false, MouseInput::up(0));

    return 1;
}

bool equal(glm::mat4 m1, glm::mat4 m2, float eps = 0.001f){
    mat4 diff = m1-m2;
    for (int x=0;x<4;x++) {
        for (int y = 0; y < 4; y++) {
            if (std::abs(diff[x][y]) > eps){
                return false;
            }
        }
    }
    return true;
}

int TestTransform(){
    auto gameObject = Engine::activeScene()->createGameObject("SomeObject");
    TINYTEST_EQUAL(gameObject->transform()->localMatrix(), gameObject->transform()->globalMatrix());
    TINYTEST_EQUAL(gameObject->transform()->localMatrix(), gameObject->transform()->localTRSInverse());
    TINYTEST_EQUAL(glm::mat4{1}, gameObject->transform()->globalMatrix());
    gameObject->transform()->setLocalPosition(vec3{1,2,3});
    mat4 transformMatrix = glm::translate(vec3{1,2,3});
    TINYTEST_EQUAL(transformMatrix, gameObject->transform()->globalMatrix());
    gameObject->transform()->setLocalPosition(vec3{0,0,0});

    std::vector<vec3> rotations;
    rotations.push_back(vec3{glm::half_pi<float>(),0,0});
    rotations.push_back(vec3{0,1,0});
    rotations.push_back(vec3{0,0,1});
    rotations.push_back(vec3{1,2,3});

    for (auto r : rotations){
        gameObject->transform()->setRotationEuler(r);
        auto expected =  yawPitchRoll(r.y, r.x, r.z);
//        cout <<glm::to_string(r) <<" becomes "<<glm::to_string(gameObject->getTransform()->getGlobalMatrix())<<" converted to euler "<<glm::to_string(eulerAngles(gameObject->getTransform()->getRotation()))<<endl;
        bool isEqual = equal(expected, gameObject->transform()->globalMatrix());
        TINYTEST_ASSERT_MSG(isEqual, (string{"error comparing matrices \n"}+
                glm::to_string(gameObject->transform()->globalMatrix())+"\n"+glm::to_string(expected)+"\n").c_str());
    }

    return 1;
}

int TestGetComponent(){
    auto gameObject = Engine::activeScene()->createGameObject("SomeObject");

    MeshRenderer *mr = gameObject->addComponent<MeshRenderer>();
    TINYTEST_EQUAL(mr, gameObject->component<MeshRenderer>());
    TINYTEST_EQUAL(nullptr, gameObject->component<Camera>());
    return 1;
}

int TestGetComponents(){
    auto gameObject = Engine::activeScene()->createGameObject("SomeObject");

    MeshRenderer *mr = gameObject->addComponent<MeshRenderer>();
    auto meshRenderers = gameObject->components<MeshRenderer>();
    TINYTEST_EQUAL(1, meshRenderers.size());
    TINYTEST_EQUAL(mr, meshRenderers.front());
    TINYTEST_EQUAL(nullptr, gameObject->component<Camera>());
    MeshRenderer *mr2 = gameObject->addComponent<MeshRenderer>();
    TINYTEST_ASSERT(mr2 != nullptr);
    meshRenderers = gameObject->components<MeshRenderer>();
    TINYTEST_EQUAL(2, meshRenderers.size());
    gameObject->destroyComponent(mr);
    meshRenderers = gameObject->components<MeshRenderer>();
    TINYTEST_EQUAL(1, meshRenderers.size());
    return 1;
}

int TestDeleteComponent(){
    class TrackComponent : public Component {
    public:
        TrackComponent(GameObject *gameObject)
                : Component(gameObject) {}

        virtual ~TrackComponent(){
            *destroyed = true;
        }

        bool * destroyed;

    };
    static bool destroyedOnClassDestruction = false;
    auto gameObject = Engine::activeScene()->createGameObject("SomeObject");

    static bool destroyed = false;

    TrackComponent *tc = gameObject->addComponent<TrackComponent>();
    tc->destroyed= &destroyed;
    TrackComponent *tc2 = gameObject->addComponent<TrackComponent>();
    tc2->destroyed = &destroyedOnClassDestruction;
    cout << "Pre destroy component"<<endl;
    gameObject->destroyComponent(tc);
    cout << "Post destroy component"<<endl;
    TINYTEST_ASSERT(destroyed);
    cout << "Pre destroy gameObject"<<endl;
    Engine::activeScene()->destroyGameObject(gameObject);
    cout << "Post destroy gameObject"<<endl;
    TINYTEST_ASSERT(destroyedOnClassDestruction);

    return 1;
}

int TestTransformComponent(){
    vector<Transform*> transforms;
    for (int i=0;i<3;i++){
        auto gameObject = Engine::activeScene()->createGameObject("SomeObject");
        auto transform = gameObject->transform();
        if (i>0){
            transform->setParent(transforms[transforms.size()-1]);
        }
        transforms.push_back(transform);
    }
    glm::vec3 offset{1,2,3};
    transforms[0]->setLocalPosition(offset);
    glm::vec3 e{ 0.00001 };
    TINYTEST_ASSERT(all(epsilonEqual(offset, transforms[1]->position(),e)));
    TINYTEST_ASSERT(all(epsilonEqual(offset, transforms[2]->position(),e)));
    transforms[1]->setLocalPosition(offset);
    TINYTEST_ASSERT(all(epsilonEqual(offset*2.0f, transforms[1]->position(),e)));
    TINYTEST_ASSERT(all(epsilonEqual(offset*2.0f, transforms[2]->position(),e)));
    transforms[1]->setLocalPosition(vec3{0});
    transforms[0]->setRotationEuler(radians(vec3{0,90,0}));
    TINYTEST_ASSERT(all(epsilonEqual(offset, transforms[1]->position(),e)));
    TINYTEST_ASSERT(all(epsilonEqual(offset, transforms[2]->position(),e)));
    transforms[1]->setLocalPosition(vec3{1,0,0});
    auto expected = glm::vec3{1,2,3-1};
    TINYTEST_ASSERT(all(epsilonEqual(expected, transforms[1]->position(),e)));
    TINYTEST_ASSERT(all(epsilonEqual(expected, transforms[2]->position(),e)));
    transforms[0]->setLocalScale(vec3{10});
    expected = glm::vec3{1,2,3-10};
    TINYTEST_ASSERT_MSG(all(epsilonEqual(expected, transforms[1]->position(),e)), glm::to_string(transforms[1]->position()).c_str());
    TINYTEST_ASSERT_MSG(all(epsilonEqual(expected, transforms[2]->position(),e)), glm::to_string(transforms[2]->position()).c_str());

    return 1;
}

bool isEqual_(glm::quat q1, glm::quat q2, float epsilon = 1e-4){
    if (q1==q2){
        return true;
    }
    quat q3 = inverse(q1) * q2;
    if (q3.x == 0 && q3.y == 0 && q3.z == 0){
        return true;
    }
    float a = angle(q3);
    return fabs(a) < epsilon;
}

int TestTransformRotationsComponent(){
    vector<Transform*> transforms;
    for (int i=0;i<3;i++){
        auto gameObject = Engine::activeScene()->createGameObject("SomeObject");
        auto transform = gameObject->transform();
        if (i>0){
            transform->setParent(transforms[transforms.size()-1]);
        }
        transforms.push_back(transform);
    }
    auto q1 = quat_cast(yawPitchRoll(10.0f,20.0f,30.0f));
    auto q0 = quat_cast(yawPitchRoll(00.0f,00.0f,00.0f));
    transforms[0]->setLocalRotation(q1);
    transforms[1]->setLocalRotation(inverse(q1));

    TINYTEST_ASSERT(isEqual_(q0, transforms[1]->rotation()));

    transforms[1]->setLocalRotation(q1);
    q1 = q1*q1; // double rotation
    TINYTEST_ASSERT(isEqual_(q1, transforms[1]->rotation()));
    return 1;
}

int TestTransformLookAt(){
    auto transform1 = Engine::activeScene()->createGameObject()->transform();
    auto transform2 = Engine::activeScene()->createGameObject()->transform();

    transform2->setLocalPosition(vec3{0,0,-10});
    transform1->lookAt(transform2);

    auto expected = quat_cast(yawPitchRoll(0.0f,radians(0.0f),0.0f));
    TINYTEST_ASSERT(isEqual_(expected, transform1->rotation()));

    transform2->setLocalPosition(vec3{10,0.0,0});
    transform1->lookAt(transform2);

    expected = quat_cast(yawPitchRoll(radians(-90.0f),0.0f,0.0f));
    TINYTEST_ASSERT_MSG(isEqual_(expected, transform1->rotation()), (glm::to_string(eulerAngles(expected))+" was "+glm::to_string(transform1->localRotationEuler())+" "+glm::to_string(transform1->rotationEuler())).c_str());

    // different up axis
    transform1->lookAt(transform2, vec3(0,0,-1));
    expected = quat_cast(yawPitchRoll(radians(-90.0f),0.0f,radians(90.0f)));
    quat q3 = inverse(transform1->rotation()) * expected;
    float a = angle(q3);
    TINYTEST_ASSERT_MSG(isEqual_(expected, transform1->rotation()), (glm::to_string(eulerAngles(expected))+" was "+glm::to_string(transform1->localRotationEuler())+" "+glm::to_string(transform1->rotationEuler())+" error "+std::to_string(a)).c_str());

    return 1;
}

int TestLoadCubemap(){
    auto img = Project::loadTextureCube("assets/textures/cubemap.png");
    return 1;
}

int TestComponentListener(){
    static set<ComponentUpdateStatus> events;
    auto listener = Engine::activeScene()->componentEvents.createListener([&](std::pair<Component*, ComponentUpdateStatus> e){
        events.insert(e.second);
    });
    auto gameObject = Engine::activeScene()->createGameObject("SomeObject");
    TINYTEST_EQUAL(0, events.size());
    auto camera = gameObject->addComponent<CameraPerspective>();
    TINYTEST_EQUAL(1, events.size());
    gameObject->setLayer(2);
    TINYTEST_EQUAL(2, events.size());
    gameObject->destroyComponent(camera);
    TINYTEST_EQUAL(3, events.size());
    return 1;
}

int TestFont(){
    Font* font = new Font();
    font->loadFntFile();

    TINYTEST_ASSERT(font->height() > 0);
    TINYTEST_ASSERT(font->width("tt") > font->width("t"));


    return 1;
}

int TestTextureAtlas(){
    TextureAtlas* textureAtlas = new TextureAtlas();
    textureAtlas->load("unittest/sprites/sprites.txt", "unittest/sprites/sprites.png");
    return 1;
}

int TestSprite(){
    auto textureAtlas = std::shared_ptr<TextureAtlas>{new TextureAtlas()};
    textureAtlas->load("unittest/sprites/sprites.txt", "unittest/sprites/sprites.png");

    auto scene = Engine::activeScene();
    auto res = scene->createPanel()->createSprite(textureAtlas, "Character Boy.png");

    return 1;
}


int TestComponentHierachy(){
    auto gameObject = Engine::activeScene()->createGameObject("SomeObject");

    auto gameObject2 = Engine::activeScene()->createGameObject("SomeObject2");
    auto camera2 = gameObject2->addComponent<Camera>();

    auto gameObject3 = Engine::activeScene()->createGameObject("SomeObject3");
    auto camera3 = gameObject3->addComponent<Camera>();

    TINYTEST_ASSERT(gameObject->component<Camera>()==nullptr);
    TINYTEST_ASSERT(gameObject->components<Camera>().size()==0);

    TINYTEST_ASSERT(gameObject->componentInParent<Camera>()==nullptr);
    TINYTEST_ASSERT(gameObject->componentsInParent<Camera>().size()==0);

    TINYTEST_ASSERT(gameObject->componentInChildren<Camera>()==nullptr);
    TINYTEST_ASSERT(gameObject->componentsInChildren<Camera>().size()==0);

    gameObject->transform()->setParent(gameObject3->transform());
    gameObject2->transform()->setParent(gameObject->transform());

    TINYTEST_ASSERT(gameObject->component<Camera>()==nullptr);
    TINYTEST_ASSERT(gameObject->components<Camera>().size()==0);

    TINYTEST_ASSERT(gameObject->componentInParent<Camera>()==camera3);
    TINYTEST_ASSERT(gameObject->componentsInParent<Camera>().size()==1);

    TINYTEST_ASSERT(gameObject->componentInChildren<Camera>()==camera2);
    TINYTEST_ASSERT(gameObject->componentsInChildren<Camera>().size()==1);

    gameObject3->transform()->setParent(gameObject2->transform());

    TINYTEST_ASSERT(gameObject->componentsInChildren<Camera>().size()==2);

    return 1;
}

int TestAABB(){
    AABB aabb1{vec3{3},vec3{5}};
    AABB aabb2{vec3{4},vec3{6}};
    AABB aabb3{vec3{7},vec3{8}};
    TINYTEST_ASSERT(aabb1.intersect(aabb2));
    TINYTEST_ASSERT(!aabb1.intersect(aabb3));
    return 1;
}

int TestKDTree(){
    KDTreeNaive kdTree;
    MeshData * meshData = MeshFactory::createUVSphereData(32, 32, 1);
    std::vector<KDTreeNodeRef> objectList;
    AABB aabb;
    for (int i=2;i<meshData->submeshSize(0);i++){

        KDTreeNodeRef ref;
        for (int j=0;j<3;j++){
            int index = meshData->submeshIndices(0)[i-j];
            ref.triangle[j] = meshData->position()[index];
            aabb.addPoint(meshData->position()[index]);
            //cout << (i-2)<<" "<< glm::to_string(meshData->position()[index])<<endl;
        }
        ref.id = i-2;
        objectList.push_back(ref);
    }

    kdTree.init(objectList, aabb);
    
    for (float x=-0.1;x<=0.1;x+=0.01f){
        for (float y=-0.1;y<=0.1;y+=0.01f){
            Ray ray{{0,0,10}, {0.00,0.00,-1}};
            KDTreeHitInfo hitInfo;
            bool res = kdTree.first_intersection(ray, hitInfo);
            if (res){
                float len = length(hitInfo.collisionPoint);
                TINYTEST_ASSERT(fabs(len-1.0) < 0.05f);
            }
        }
    }
    
    delete meshData;
    return 1;
}


int TestRay(){
    Ray ray{vec3{0,0,10},vec3{0,0,-1}};
    vec3 intersectionPoint;
    bool res;
    res = ray.intersect(vec3(-1,-1,0), vec3(-1,3,0), vec3(3,-1,0), intersectionPoint);
    TINYTEST_ASSERT(res);
    res = ray.intersect(vec3(0), vec3(0,1,0), vec3(1,0,0), intersectionPoint);
    TINYTEST_ASSERT(res);
    TINYTEST_ASSERT(intersectionPoint == vec3(0,0,0));
    res = ray.intersect(vec3(0,1,0), vec3(0), vec3(1,0,0), intersectionPoint);
    TINYTEST_ASSERT(res);
    TINYTEST_ASSERT(intersectionPoint == vec3(0,0,0));
    res = ray.intersect(vec3(0,1,0), vec3(1,0,0), vec3(0), intersectionPoint);
    TINYTEST_ASSERT(res);
    TINYTEST_ASSERT(intersectionPoint == vec3(0,0,0));
    ray = Ray{{1,1,10},{0,0,-1}};
    res = ray.intersect(vec3(0,0,0), vec3(2,0,0), vec3(0,2,0), intersectionPoint);
    TINYTEST_ASSERT(res);
    TINYTEST_ASSERT(intersectionPoint == vec3(1,1,0));
    return 1;
}
