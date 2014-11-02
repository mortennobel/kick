//
//  Engine.h
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 5/21/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once

#include <iostream>
#include <vector>
#include "kick/scene/scene.h"
#include "kick/context/context.h"
#include "kick/core/event_listener.h"
#include "kick/core/event.h"
#include "kick/core/project.h"
#include "kick/core/engine.h"
#include "kick/core/mouse_input.h"
#include "kick/core/key_input.h"
#include "kick/core/touch_input.h"
#include "kick/core/default_key_handler.h"
#include "kick/core/event_queue.h"

namespace kick {
    
    struct EngineConfig {
        bool shadows = false;
        int maxNumerOfLights = 3;
    };

    class Engine {
        friend class Project;
    public:
        static void init(int &argc, char **argv, const WindowConfig& config = WindowConfig::plain);
        static Scene *getActiveScene() { return instance->activeScene; }
        static void setActiveScene(Scene *scene) { instance->activeScene = scene; }
        static Scene * createScene(const std::string &name);
        static std::vector<Scene>::const_iterator begin();
        static std::vector<Scene>::const_iterator end();

        static EngineConfig& getConfig(){ return instance->config; }
        static Context* getContext();
        static DefaultKeyHandler & getDefaultKeyHandler();
        static void startMainLoop();
        static void startFrame();
        static void update();
        static void render();

        static EventQueue & getEventQueue();

    private:
        static Engine* instance;
        EngineConfig config;
        EventQueue eventQueue;
        Engine(int &argc, char **argv, const WindowConfig& config = WindowConfig::plain);
        float tickStartTime;

        Project project;
        std::vector<Scene> scenes;
        EngineUniforms engineUniforms;
        Scene *activeScene = nullptr;
        Context* context = nullptr;
        DefaultKeyHandler defaultKeyHandler;
    };
};
