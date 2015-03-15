//
//  Engine.cpp
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 5/21/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/core/engine.h"
#include "kick/core/time.h"
#include "kick/core/debug.h"
#include "kick/context/sdl2_context.h"
#include <iostream>

using namespace std;

namespace kick {

    Engine* Engine::instance = nullptr;

    Engine::Engine(int &argc, char **argv,const WindowConfig& config)
    : mContext(new SDL2Context()), tickStartTime{Time::total()} {
        instance = this;
        mContext->init(argc, argv);
        mContext->showWindow(config);
        printf("%s (%s)\n",
                glGetString(GL_RENDERER),  // e.g. Intel HD Graphics 3000 OpenGL Engine
                glGetString(GL_VERSION)    // e.g. 3.2  INTEL-8.0.61
        );
        createScene("defaultScene");
        mContext->contextSurfaceSize.registerSyncValue(engineUniforms.viewportDimension);
        engineUniforms.viewportDimension.setValue(mContext->getContextSurfaceDim());
#ifdef DEBUG
		printf("Working dir %s\n", Debug::workingDir().c_str());
#endif
    }

    void Engine::startMainLoop(){
        instance->mContext->mainLoop();
    }

    void Engine::update(){
        float now = Time::total();
        Time::deltaTime = now - instance->tickStartTime;
        instance->tickStartTime = now;
        Time::frame++;

        instance->eventQueue.run();

        instance->mDefaultKeyHandler.handleKeyPress(instance);
        instance->mActiveScene->update();
    }
    
    void Engine::render(){
        instance->mActiveScene->render(&instance->engineUniforms);
        instance->mContext->swapBuffer();
#ifdef DEBUG
        printOpenGLError();
#endif
    }
    
    Scene * Engine::createScene(const std::string & name){
        instance->scenes.push_back(Scene{name});
        Scene * scene = &(instance->scenes.back());;
        if (!instance->mActiveScene){
            instance->mActiveScene = scene;
        }
        return scene;
    }
    
    std::vector<Scene>::const_iterator Engine::begin()  {
        return instance->scenes.begin();
    }
    std::vector<Scene>::const_iterator Engine::end() {
        return instance->scenes.end();
    }
    
    Context* Engine::context(){
        return instance->mContext;
    }

    void Engine::startFrame() {
        KeyInput::reset();
        MouseInput::reset();
        TouchInput::reset();
    }

    DefaultKeyHandler &Engine::defaultKeyHandler() {
        return instance->mDefaultKeyHandler;
    }

    void Engine::init(int &argc, char **argv, WindowConfig const &config) {
        cout << "kick "<< headerVersion()<<" (lib "<< libVersion()<<")"<<endl;

        assert(instance == nullptr);
        new Engine(argc, argv, config);
    }

    EventQueue &Engine::getEventQueue() {
        return instance->eventQueue;
    }

    std::string Engine::libVersion() {
        return headerVersion();
    }
}