//
//  Engine.cpp
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 5/21/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/core/engine.h"
#include "kick/core/time.h"
#include "kick/context/sdl2_context.h"
#include <iostream>

using namespace std;

namespace kick {

    Engine* Engine::instance = nullptr;

    Engine::Engine(int &argc, char **argv,const WindowConfig& config)
    :context(new SDL2Context()), tickStartTime{Time::get()} {
        instance = this;
        context->init(argc, argv);
        context->showWindow(config);
        printf("%s (%s)\n",
                glGetString(GL_RENDERER),  // e.g. Intel HD Graphics 3000 OpenGL Engine
                glGetString(GL_VERSION)    // e.g. 3.2  INTEL-8.0.61
        );
        createScene("defaultScene");
        context->contextSurfaceSize.registerSyncValue(engineUniforms.viewportDimension);
        engineUniforms.viewportDimension.setValue(context->getContextSurfaceDim());
    }

    void Engine::startMainLoop(){
        instance->context->mainLoop();
    }

    void Engine::update(){
        float now = Time::get();
        Time::deltaTime = now - instance->tickStartTime;
        instance->tickStartTime = now;
        Time::frame++;

        instance->eventQueue.run();

        instance->defaultKeyHandler.handleKeyPress(instance);
        instance->activeScene->update();
    }
    
    void Engine::render(){
        instance->activeScene->render(&instance->engineUniforms);
        instance->context->swapBuffer();
#ifdef DEBUG
        printOpenGLError();
#endif
    }
    
    Scene * Engine::createScene(const std::string & name){
        instance->scenes.push_back(Scene{name});
        Scene * scene = &(instance->scenes.back());;
        if (!instance->activeScene){
            instance->activeScene = scene;
        }
        return scene;
    }
    
    std::vector<Scene>::const_iterator Engine::begin()  {
        return instance->scenes.begin();
    }
    std::vector<Scene>::const_iterator Engine::end() {
        return instance->scenes.end();
    }
    
    Context* Engine::getContext(){
        return instance->context;
    }

    void Engine::startFrame() {
        KeyInput::reset();
        MouseInput::reset();
        TouchInput::reset();
    }

    DefaultKeyHandler &Engine::getDefaultKeyHandler() {
        return instance->defaultKeyHandler;
    }

    void Engine::init(int &argc, char **argv, WindowConfig const &config) {
        assert(instance == nullptr);
        new Engine(argc, argv, config);
    }

    EventQueue &Engine::getEventQueue() {
        return instance->eventQueue;
    }
}