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

namespace kick {
    
    struct EngineConfig {
        bool shadows = false;
        int maxNumerOfLights = 3;
    };
    
    class Engine {
        friend class Project;
    public:
        static Engine* init(int &argc, char **argv, const WindowConfig& config = WindowConfig::plain);
        Scene *getActiveScene() { return activeScene; }
        void setActiveScene(Scene *scene) { activeScene = scene; }
        Scene * createScene(const std::string &name);
        std::vector<Scene>::const_iterator begin() const;
        std::vector<Scene>::const_iterator end() const;
        static Engine* instance;
        EngineConfig config;
        Context* getContext();
        const MouseInput& getMouseInput();
        const KeyInput& getKeyInput();
        DefaultKeyHandler & getDefaultKeyHandler();
        void startMainLoop();
        void startFrame();
        void update();
        void render();
    private:
        Engine(int &argc, char **argv, const WindowConfig& config = WindowConfig::plain);
        float tickStartTime;

        Project project;
        std::vector<Scene> scenes;
        EngineUniforms engineUniforms;
        Scene *activeScene = nullptr;
        Context* context = nullptr;
        MouseInput mouseInput;
        KeyInput keyInput;
        TouchInput touchInput;
        DefaultKeyHandler defaultKeyHandler;
    };
};
