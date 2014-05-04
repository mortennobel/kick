//
//  SDL2_Context.h
//  KickCPP
//
//  Created by morten on 8/10/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once
#include "context.h"
#include <SDL2/SDL.h>

namespace kick {
    class SDL2Context : public Context {
    public:
        SDL2Context();
        virtual ~SDL2Context();
        virtual bool init(int &argc, char **argv);
        virtual bool showWindow(const WindowConfig& config = WindowConfig::plain);
        virtual void swapBuffer();
        virtual void mainLoop();
        std::string getBasePath();

        virtual bool isFullscreen();

        virtual void setFullscreen(bool fullscreen);

        void step();
    private:
        void handleMouseMotion(SDL_MouseMotionEvent event);
        void handleMouseButton(SDL_MouseButtonEvent event, bool buttonDown);
        void handleMouseWheel(SDL_MouseWheelEvent event);
        void handleKey(SDL_KeyboardEvent event, bool keyDown);
        Uint32 timeLeft();
        Uint32 nextTime;
        Uint32 tickInterval = 16; // 60 fps
        SDL_Window *window = nullptr;
        SDL_GLContext context = nullptr;
        std::string basePath;
        bool fullscreen = false;
    };
};