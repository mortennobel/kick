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
#include "kick.h"

#include "glm/gtx/string_cast.hpp"


using namespace kick;
using namespace std;
using namespace glm;

Engine *engine;

void initEngine(int& argc, char** argv){
    // Manually create engine instance
    //GLUTContext cont;

    ::engine = Engine::init(argc, argv);
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

    auto shaderAttributes = shader->getShaderAttributes();
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

    meshData->setSubmesh(0, {
            0,1,2,
            3,4,5,
            6,7,8,
            9,10,11
    }, MeshType::Triangles);

    AABB aabb = meshData->getAabb();
    TINYTEST_ASSERT(aabb.min == p0);
    vec3 vec3max{1,std::sqrt(0.75f),std::sqrt(0.75f)};
    TINYTEST_ASSERT(aabb.max == vec3max);

    meshData->recomputeNormals();
    TINYTEST_ASSERT(meshData->getNormal().size() == meshData->getPosition().size());
    for (auto normal : meshData->getNormal()){
        TINYTEST_ASSERT(normal != vec3(0));
    }

    auto interleavedData = meshData->getInterleavedData();
    auto interleavedFormat = meshData->getInterleavedFormat();
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
        TINYTEST_ASSERT(isTransparent == shader->getBlend());

    }
    return 1;
}

int TestKeyInputButton(){
    KeyInput keyInput;
    TINYTEST_EQUAL(false, keyInput.pressed(Key::x));
    TINYTEST_EQUAL(false, keyInput.down(Key::x));
    TINYTEST_EQUAL(false, keyInput.up(Key::x));

    keyInput.pressBegin(Key::x);
    TINYTEST_EQUAL(true, keyInput.pressed(Key::x));
    TINYTEST_EQUAL(true, keyInput.down(Key::x));
    TINYTEST_EQUAL(false, keyInput.up(Key::x));

    keyInput.reset();
    TINYTEST_EQUAL(true, keyInput.pressed(Key::x));
    TINYTEST_EQUAL(false, keyInput.down(Key::x));
    TINYTEST_EQUAL(false, keyInput.up(Key::x));

    keyInput.pressEnd(Key::x);
    TINYTEST_EQUAL(false, keyInput.pressed(Key::x));
    TINYTEST_EQUAL(false, keyInput.down(Key::x));
    TINYTEST_EQUAL(true, keyInput.up(Key::x));

    keyInput.reset();
    TINYTEST_EQUAL(false, keyInput.pressed(Key::x));
    TINYTEST_EQUAL(false, keyInput.down(Key::x));
    TINYTEST_EQUAL(false, keyInput.up(Key::x));

    return 1;
}

int TestKeyMapping(){
    KeyMapping keyMappingX{Key::x};

    KeyInput keyInput;
    TINYTEST_EQUAL(false, keyInput.pressed(keyMappingX));
    TINYTEST_EQUAL(false, keyInput.down(keyMappingX));

    keyInput.pressBegin(Key::x);
    TINYTEST_EQUAL(true, keyInput.pressed(keyMappingX));
    TINYTEST_EQUAL(true, keyInput.down(keyMappingX));

    keyInput.reset();
    TINYTEST_EQUAL(true, keyInput.pressed(keyMappingX));
    TINYTEST_EQUAL(false, keyInput.down(keyMappingX));

    keyInput.pressEnd(Key::x);
    TINYTEST_EQUAL(false, keyInput.pressed(keyMappingX));
    TINYTEST_EQUAL(false, keyInput.down(keyMappingX));

    keyInput.reset();
    TINYTEST_EQUAL(false, keyInput.pressed(keyMappingX));
    TINYTEST_EQUAL(false, keyInput.down(keyMappingX));

    return 1;
}

int TestKeyMappingMulti(){
    KeyMapping keyMappingXY{Key::x, Key::y};

    KeyInput keyInput;
    TINYTEST_EQUAL(false, keyInput.pressed(keyMappingXY));
    TINYTEST_EQUAL(false, keyInput.down(keyMappingXY));

    keyInput.pressBegin(Key::x);
    TINYTEST_EQUAL(false, keyInput.pressed(keyMappingXY));
    TINYTEST_EQUAL(false, keyInput.down(keyMappingXY));

    keyInput.reset();
    TINYTEST_EQUAL(false, keyInput.pressed(keyMappingXY));
    TINYTEST_EQUAL(false, keyInput.down(keyMappingXY));

    keyInput.reset();

    keyInput.pressBegin(Key::y);
    TINYTEST_EQUAL(true, keyInput.pressed(keyMappingXY));
    TINYTEST_EQUAL(true, keyInput.down(keyMappingXY));

    keyInput.reset();
    TINYTEST_EQUAL(true, keyInput.pressed(keyMappingXY));
    TINYTEST_EQUAL(false, keyInput.down(keyMappingXY));

    keyInput.pressEnd(Key::x);
    TINYTEST_EQUAL(false, keyInput.pressed(keyMappingXY));
    TINYTEST_EQUAL(false, keyInput.down(keyMappingXY));

    keyInput.reset();
    TINYTEST_EQUAL(false, keyInput.pressed(keyMappingXY));
    TINYTEST_EQUAL(false, keyInput.down(keyMappingXY));

    return 1;
}


int TestMouseInputButton(){
    MouseInput mouseInput;
    TINYTEST_EQUAL(false, mouseInput.pressed(0));
    TINYTEST_EQUAL(false, mouseInput.down(1));
    TINYTEST_EQUAL(false, mouseInput.up(2));
    TINYTEST_EQUAL(false, mouseInput.up(3));
    TINYTEST_EQUAL(false, mouseInput.up(4));

    mouseInput.buttonPressStarted(0);
    TINYTEST_EQUAL(true, mouseInput.pressed(0));
    TINYTEST_EQUAL(true, mouseInput.down(0));
    TINYTEST_EQUAL(false, mouseInput.up(0));

    mouseInput.reset();
    TINYTEST_EQUAL(true, mouseInput.pressed(0));
    TINYTEST_EQUAL(false, mouseInput.down(0));
    TINYTEST_EQUAL(false, mouseInput.up(0));

    mouseInput.buttonPressEnded(0);
    TINYTEST_EQUAL(false, mouseInput.pressed(0));
    TINYTEST_EQUAL(false, mouseInput.down(0));
    TINYTEST_EQUAL(true, mouseInput.up(0));

    mouseInput.reset();
    TINYTEST_EQUAL(false, mouseInput.pressed(0));
    TINYTEST_EQUAL(false, mouseInput.down(0));
    TINYTEST_EQUAL(false, mouseInput.up(0));

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
    auto gameObject = Engine::instance->getActiveScene()->createGameObject("SomeObject");
    TINYTEST_EQUAL(gameObject->getTransform()->getLocalMatrix(), gameObject->getTransform()->getGlobalMatrix());
    TINYTEST_EQUAL(gameObject->getTransform()->getLocalMatrix(), gameObject->getTransform()->getLocalTRSInverse());
    TINYTEST_EQUAL(glm::mat4{1}, gameObject->getTransform()->getGlobalMatrix());
    gameObject->getTransform()->setLocalPosition(vec3{1,2,3});
    mat4 transformMatrix = glm::translate(vec3{1,2,3});
    TINYTEST_EQUAL(transformMatrix, gameObject->getTransform()->getGlobalMatrix());
    gameObject->getTransform()->setLocalPosition(vec3{0,0,0});

    std::vector<vec3> rotations;
    rotations.push_back(vec3{glm::half_pi<float>(),0,0});
    rotations.push_back(vec3{0,1,0});
    rotations.push_back(vec3{0,0,1});
    rotations.push_back(vec3{1,2,3});

    for (auto r : rotations){
        gameObject->getTransform()->setRotationEuler(r);
        auto expected =  yawPitchRoll(r.y, r.x, r.z);
//        cout <<glm::to_string(r) <<" becomes "<<glm::to_string(gameObject->getTransform()->getGlobalMatrix())<<" converted to euler "<<glm::to_string(eulerAngles(gameObject->getTransform()->getRotation()))<<endl;
        bool isEqual = equal(expected, gameObject->getTransform()->getGlobalMatrix());
        TINYTEST_ASSERT_MSG(isEqual, (string{"error comparing matrices \n"}+
                glm::to_string( gameObject->getTransform()->getGlobalMatrix())+"\n"+glm::to_string(expected)+"\n").c_str());
    }

    return 1;
}

int TestGetComponent(){
    auto gameObject = Engine::instance->getActiveScene()->createGameObject("SomeObject");

    MeshRenderer *mr = gameObject->addComponent<MeshRenderer>();
    TINYTEST_EQUAL(mr, gameObject->getComponent<MeshRenderer>());
    TINYTEST_EQUAL(nullptr, gameObject->getComponent<Camera>());
    return 1;
}

int TestGetComponents(){
    auto gameObject = Engine::instance->getActiveScene()->createGameObject("SomeObject");

    MeshRenderer *mr = gameObject->addComponent<MeshRenderer>();
    auto meshRenderers = gameObject->getComponents<MeshRenderer>();
    TINYTEST_EQUAL(1, meshRenderers.size());
    TINYTEST_EQUAL(mr, meshRenderers.front());
    TINYTEST_EQUAL(nullptr, gameObject->getComponent<Camera>());
    MeshRenderer *mr2 = gameObject->addComponent<MeshRenderer>();
    TINYTEST_ASSERT(mr2 != nullptr);
    meshRenderers = gameObject->getComponents<MeshRenderer>();
    TINYTEST_EQUAL(2, meshRenderers.size());
    gameObject->destroyComponent(mr);
    meshRenderers = gameObject->getComponents<MeshRenderer>();
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
    auto gameObject = Engine::instance->getActiveScene()->createGameObject("SomeObject");

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
    Engine::instance->getActiveScene()->destroyGameObject(gameObject);
    cout << "Post destroy gameObject"<<endl;
    TINYTEST_ASSERT(destroyedOnClassDestruction);

    return 1;
}

int TestTransformComponent(){
    vector<Transform*> transforms;
    for (int i=0;i<3;i++){
        auto gameObject = Engine::instance->getActiveScene()->createGameObject("SomeObject");
        auto transform = gameObject->getTransform();
        if (i>0){
            transform->setParent(transforms[transforms.size()-1]);
        }
        transforms.push_back(transform);
    }
    glm::vec3 offset{1,2,3};
    transforms[0]->setLocalPosition(offset);
    glm::vec3 e{ 0.00001 };
    TINYTEST_ASSERT(all(epsilonEqual(offset, transforms[1]->getPosition(),e)));
    TINYTEST_ASSERT(all(epsilonEqual(offset, transforms[2]->getPosition(),e)));
    transforms[1]->setLocalPosition(offset);
    TINYTEST_ASSERT(all(epsilonEqual(offset*2.0f, transforms[1]->getPosition(),e)));
    TINYTEST_ASSERT(all(epsilonEqual(offset*2.0f, transforms[2]->getPosition(),e)));
    transforms[1]->setLocalPosition(vec3{0});
    transforms[0]->setRotationEuler(radians(vec3{0,90,0}));
    TINYTEST_ASSERT(all(epsilonEqual(offset, transforms[1]->getPosition(),e)));
    TINYTEST_ASSERT(all(epsilonEqual(offset, transforms[2]->getPosition(),e)));
    transforms[1]->setLocalPosition(vec3{1,0,0});
    auto expected = glm::vec3{1,2,3-1};
    TINYTEST_ASSERT(all(epsilonEqual(expected, transforms[1]->getPosition(),e)));
    TINYTEST_ASSERT(all(epsilonEqual(expected, transforms[2]->getPosition(),e)));
    transforms[0]->setLocalScale(vec3{10});
    expected = glm::vec3{1,2,3-10};
    TINYTEST_ASSERT_MSG(all(epsilonEqual(expected, transforms[1]->getPosition(),e)), glm::to_string(transforms[1]->getPosition()).c_str());
    TINYTEST_ASSERT_MSG(all(epsilonEqual(expected, transforms[2]->getPosition(),e)), glm::to_string(transforms[2]->getPosition()).c_str());

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
        auto gameObject = Engine::instance->getActiveScene()->createGameObject("SomeObject");
        auto transform = gameObject->getTransform();
        if (i>0){
            transform->setParent(transforms[transforms.size()-1]);
        }
        transforms.push_back(transform);
    }
    auto q1 = quat_cast(yawPitchRoll(10.0f,20.0f,30.0f));
    auto q0 = quat_cast(yawPitchRoll(00.0f,00.0f,00.0f));
    transforms[0]->setLocalRotation(q1);
    transforms[1]->setLocalRotation(inverse(q1));

    TINYTEST_ASSERT(isEqual_(q0,transforms[1]->getRotation()));

    transforms[1]->setLocalRotation(q1);
    q1 = q1*q1; // double rotation
    TINYTEST_ASSERT(isEqual_(q1,transforms[1]->getRotation()));
    return 1;
}

int TestTransformLookAt(){
    auto transform1 = Engine::instance->getActiveScene()->createGameObject()->getTransform();
    auto transform2 = Engine::instance->getActiveScene()->createGameObject()->getTransform();

    transform2->setLocalPosition(vec3{0,0,-10});
    transform1->lookAt(transform2);

    auto expected = quat_cast(yawPitchRoll(0.0f,radians(0.0f),0.0f));
    TINYTEST_ASSERT(isEqual_(expected,transform1->getRotation()));

    transform2->setLocalPosition(vec3{10,0.0,0});
    transform1->lookAt(transform2);

    expected = quat_cast(yawPitchRoll(radians(-90.0f),0.0f,0.0f));
    TINYTEST_ASSERT_MSG(isEqual_(expected,transform1->getRotation()), (glm::to_string(eulerAngles(expected))+" was "+glm::to_string(transform1->getLocalRotationEuler())+" "+glm::to_string(transform1->getRotationEuler())).c_str());

    // different up axis
    transform1->lookAt(transform2, vec3(0,0,-1));
    expected = quat_cast(yawPitchRoll(radians(-90.0f),0.0f,radians(90.0f)));
    quat q3 = inverse(transform1->getRotation()) * expected;
    float a = angle(q3);
    TINYTEST_ASSERT_MSG(isEqual_(expected,transform1->getRotation()), (glm::to_string(eulerAngles(expected))+" was "+glm::to_string(transform1->getLocalRotationEuler())+" "+glm::to_string(transform1->getRotationEuler())+" error "+std::to_string(a)).c_str());

    return 1;
}

int TestLoadCubemap(){
    auto img = Project::loadTextureCube("assets/textures/cubemap.png");
    return 1;
}

int TestComponentListener(){
    static set<ComponentUpdateStatus> events;
    auto listener = Engine::instance->getActiveScene()->componentEvents.createListener([&](std::pair<Component*, ComponentUpdateStatus> e){
        events.insert(e.second);
    });
    auto gameObject = Engine::instance->getActiveScene()->createGameObject("SomeObject");
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

    auto scene = Engine::instance->getActiveScene();
    auto res = scene->createPanel()->createSprite(textureAtlas, "Character Boy.png");

    return 1;
}


int TestComponentHierachy(){
    auto gameObject = Engine::instance->getActiveScene()->createGameObject("SomeObject");

    auto gameObject2 = Engine::instance->getActiveScene()->createGameObject("SomeObject2");
    auto camera2 = gameObject2->addComponent<Camera>();

    auto gameObject3 = Engine::instance->getActiveScene()->createGameObject("SomeObject3");
    auto camera3 = gameObject3->addComponent<Camera>();

    TINYTEST_ASSERT(gameObject->getComponent<Camera>()==nullptr);
    TINYTEST_ASSERT(gameObject->getComponents<Camera>().size()==0);

    TINYTEST_ASSERT(gameObject->getComponentInParent<Camera>()==nullptr);
    TINYTEST_ASSERT(gameObject->getComponentsInParent<Camera>().size()==0);

    TINYTEST_ASSERT(gameObject->getComponentInChildren<Camera>()==nullptr);
    TINYTEST_ASSERT(gameObject->getComponentsInChildren<Camera>().size()==0);

    gameObject->getTransform()->setParent(gameObject3->getTransform());
    gameObject2->getTransform()->setParent(gameObject->getTransform());

    TINYTEST_ASSERT(gameObject->getComponent<Camera>()==nullptr);
    TINYTEST_ASSERT(gameObject->getComponents<Camera>().size()==0);

    TINYTEST_ASSERT(gameObject->getComponentInParent<Camera>()==camera3);
    TINYTEST_ASSERT(gameObject->getComponentsInParent<Camera>().size()==1);

    TINYTEST_ASSERT(gameObject->getComponentInChildren<Camera>()==camera2);
    TINYTEST_ASSERT(gameObject->getComponentsInChildren<Camera>().size()==1);

    gameObject3->getTransform()->setParent(gameObject2->getTransform());

    TINYTEST_ASSERT(gameObject->getComponentsInChildren<Camera>().size()==2);

    return 1;
}

