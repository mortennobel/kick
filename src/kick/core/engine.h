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
        static Scene *activeScene() { return instance->mActiveScene; }
        static void setActiveScene(Scene *scene) { instance->mActiveScene = scene; }
        static Scene * createScene(const std::string &name);
        static std::vector<Scene>::const_iterator begin();
        static std::vector<Scene>::const_iterator end();

        static const EngineConfig& config(){ return instance->mConfig; }
        static Context* context();
        static DefaultKeyHandler &defaultKeyHandler();
        static void startMainLoop();
        static void startFrame();
        static void update();
        static void render();

        // return the version number of the header
        inline static std::string headerVersion(){
            return std::string("0.0.1")
#ifdef DEBUG
                +"d"
#endif
#ifdef NDEBUG
                +"n"
#endif
            ;
        }

        static std::string libVersion();

        static EventQueue & getEventQueue();

    private:
        static Engine* instance;
        EngineConfig mConfig;
        EventQueue eventQueue;
        Engine(int &argc, char **argv, const WindowConfig& config = WindowConfig::plain);
        float tickStartTime;

        Project project;
        std::vector<Scene> scenes;
        EngineUniforms engineUniforms;
        Scene *mActiveScene = nullptr;
        Context* mContext = nullptr;
        DefaultKeyHandler mDefaultKeyHandler;
    };
};
