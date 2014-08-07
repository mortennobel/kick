//
//  PlatformBinding.cpp
//  KickCPP
//
//  Created by morten on 8/10/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/context/sdl2_context.h"
#include "kick/core/mouse_input.h"
#include "kick/core/key_input.h"
#include "kick/core/engine.h"

using namespace std;
using namespace glm;

namespace kick {
    SDL2Context *context = nullptr;


    SDL2Context::SDL2Context(){
        context = this;
        /*
        char *base_path = SDL_GetBasePath();
        if (base_path) {
            basePath = string{base_path};
            SDL_free(base_path);
        } else {
            basePath = "./";
        }
         */
#ifndef EMSCRIPTEN
        SDL_version compiled;
        SDL_version linked;

        SDL_VERSION(&compiled);
        SDL_GetVersion(&linked);
        if (compiled.major != linked.major ||
                compiled.minor != linked.minor ||
                compiled.patch != linked.patch ){
            printf("Compiled against SDL version %d.%d.%d ...\n", compiled.major, compiled.minor, compiled.patch);
            printf("Compiled linked against SDL version %d.%d.%d.\n", linked.major, linked.minor, linked.patch);
        } else {
            printf("SDL version: %d.%d.%d\n", compiled.major, compiled.minor, compiled.patch);
        }
#endif

        basePath = "./";
    }
    
    SDL2Context::~SDL2Context(){

        if (context){
            SDL_GL_DeleteContext(context);
        }
#ifndef EMSCRIPTEN
        if (window){
            SDL_DestroyWindow(window);
        }
#endif
        SDL_Quit();

    }
    
    bool SDL2Context::init(int &argc, char **argv){
#ifndef EMSCRIPTEN
        SDL_SetMainReady();
        bool res = SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) >= 0;
#else
        bool res = SDL_Init(SDL_INIT_EVERYTHING) >= 0;
#endif

#ifndef EMSCRIPTEN
        if (SDL_IsScreenSaverEnabled()){
            SDL_DisableScreenSaver();
        }
#endif

        return res;
    }
    
    bool SDL2Context::showWindow(const WindowConfig& config){
        const char *windowName = config.name.c_str();
        int width = config.width;
        int height = config.height;
        int depthSize = config.depthBufferSize;
        
        contextSurfaceDim = glm::vec2(width, height);
        if (depthSize>0){
            /* Turn on double buffering with a 24bit Z buffer.
             * You may need to change this to 16 or 32 for your system */
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depthSize);
        }

#ifdef EMSCRIPTEN
        cout << "SDL_SetVideoMode("<<width<<height<<endl;
        SDL_Surface *screen = SDL_SetVideoMode(width, height, 32, SDL_OPENGL);
        window = nullptr;
#else
        /* Request opengl 3.2 context.
         * SDL doesn't have the ability to choose which profile at this time of writing,
         * but it should default to the core profile */

#ifdef KICK_CONTEXT_ES2
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
#endif

        /* Create our window centered at 512x512 resolution */
        window = SDL_CreateWindow(windowName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
        if (!window)
            return false;
        
        context = SDL_GL_CreateContext(window);
#endif
        return true;
    }
    
    void SDL2Context::swapBuffer(){
        SDL_GL_SwapWindow(window);
    }
    
    void SDL2Context::step(){
        Engine::instance->update();
        Engine::instance->render();
    }

    bool SDL2Context::tick(){
        bool quit = false;
        SDL_Event event;
        Engine::instance->startFrame();
        while(SDL_PollEvent(&event))
        {
            switch (event.type) {
                case SDL_QUIT:
                    quit = 1;
                    break;
                case SDL_WINDOWEVENT_RESIZED:
                    break;
#ifndef EMSCRIPTEN
                case SDL_APP_TERMINATING: /**< The application is being terminated by the OS
                                          Called on iOS in applicationWillTerminate()
                                          Called on Android in onDestroy()
                                          */
                    break;
                case SDL_APP_LOWMEMORY: /**< The application is low on memory, free memory if possible.
                                        Called on iOS in applicationDidReceiveMemoryWarning()
                                        Called on Android in onLowMemory()
                                        */
                    break;
                case SDL_APP_WILLENTERBACKGROUND: /**< The application is about to enter the background
                                                  Called on iOS in applicationWillResignActive()
                                                  Called on Android in onPause()
                                                  */
                    break;
                case SDL_APP_DIDENTERBACKGROUND: /**< The application did enter the background and may not get CPU for some time
                                                 Called on iOS in applicationDidEnterBackground()
                                                 Called on Android in onPause()
                                                 */
                    break;
                case SDL_APP_WILLENTERFOREGROUND: /**< The application is about to enter the foreground
                                                  Called on iOS in applicationWillEnterForeground()
                                                  Called on Android in onResume()
                                                  */
                    break;
                case SDL_APP_DIDENTERFOREGROUND:  /**< The application is now interactive
                                                  Called on iOS in applicationDidBecomeActive()
                                                  Called on Android in onResume()
                                                  */
                    break;
                case SDL_WINDOWEVENT: /**< Window state change */
                    break;
                case SDL_SYSWMEVENT: /**< System specific event */
                    break;
#endif
                case SDL_KEYDOWN:
                    handleKey(event.key, true);
                    break;
                case SDL_KEYUP:
                    handleKey(event.key, false);
                    break;
                case SDL_TEXTEDITING:/**< Keyboard text editing (composition) */
                    break;
                case SDL_TEXTINPUT:
                    break;
                case SDL_MOUSEMOTION:
                    handleMouseMotion(event.motion);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    handleMouseButton(event.button, true);
                    break;
                case SDL_MOUSEBUTTONUP:
                    handleMouseButton(event.button, false);
                    break;
                case SDL_MOUSEWHEEL:
                    handleMouseWheel(event.wheel);
                    break;
#ifndef EMSCRIPTEN
                case SDL_JOYAXISMOTION:
                    break;
                case SDL_JOYBALLMOTION:
                    break;
                case SDL_JOYHATMOTION:
                    break;
                case SDL_JOYBUTTONDOWN:
                    break;
                case SDL_JOYBUTTONUP:
                    break;
                case SDL_JOYDEVICEADDED:
                    break;
                case SDL_JOYDEVICEREMOVED:
                    break;
                case SDL_CONTROLLERAXISMOTION:
                    break;
                case SDL_CONTROLLERBUTTONDOWN:
                    break;
                case SDL_CONTROLLERBUTTONUP:
                    break;
                case SDL_CONTROLLERDEVICEADDED:
                    break;
                case SDL_CONTROLLERDEVICEREMOVED:
                    break;
                case SDL_CONTROLLERDEVICEREMAPPED:
                    break;
                case SDL_FINGERDOWN: /* Touch events */
                    break;
                case SDL_FINGERUP:
                    break;
                case SDL_FINGERMOTION:
                    break;
                case SDL_DOLLARGESTURE: /* Gesture events */
                    break;
                case SDL_DOLLARRECORD:
                    break;
                case SDL_MULTIGESTURE:
                    break;
                case SDL_CLIPBOARDUPDATE:
                    break;
                case SDL_DROPFILE: /**< The system requests a file open */
                    break;
                default:
                    // unhandled
                    break;
#endif
            }
        }
        step();
        return quit;
    }

#ifdef EMSCRIPTEN
    void kick_emscripten_tick(){
        context->tick();
    }
#endif

    void SDL2Context::mainLoop(){
#ifdef EMSCRIPTEN
        int fps = 0;
        int simulate_infinite_loop = 1;
        emscripten_set_main_loop(kick_emscripten_tick, fps, simulate_infinite_loop);
#else
        /* An SDL_Event */
        bool quit = false;
        nextTime = SDL_GetTicks() + tickInterval;
        while (!quit) {
            quit = tick();
            SDL_Delay(timeLeft());
            nextTime += tickInterval;
        }
#endif
    }
    
    Uint32 SDL2Context::timeLeft(){
        Uint32 now;
        
        now = SDL_GetTicks();
        if(nextTime <= now)
            return 0;
        else
            return nextTime - now;
    }
    
    std::string SDL2Context::getBasePath(){
        return basePath;
    }
    
    void SDL2Context::handleKey(SDL_KeyboardEvent event, bool keyDown){
        Key key = static_cast<Key>(event.keysym.sym);
        if (keyDown){
            if (event.repeat){
                return;
            }
            keyInput->pressBegin(key);
        } else {
            keyInput->pressEnd(key);
        }
    }
    void SDL2Context::handleMouseMotion(SDL_MouseMotionEvent event){
        mouseInput->setPosition(ivec2(event.x,event.y));
        mouseInput->setPositionDelta(ivec2(event.xrel,event.yrel));
    }
    
    void SDL2Context::handleMouseButton(SDL_MouseButtonEvent event, bool buttonDown){
        if (buttonDown){
            mouseInput->buttonPressStarted(event.button-1);
        } else {
            mouseInput->buttonPressEnded(event.button-1);
        }
#ifndef EMSCRIPTEN
        mouseInput->setClicks(event.clicks);
#endif
    }
    
    void  SDL2Context::handleMouseWheel(SDL_MouseWheelEvent event){
        mouseInput->setMouseWheelDelta(ivec2(event.x, event.y));
    }

    bool SDL2Context::isFullscreen() {
        return fullscreen;
    }

    void SDL2Context::setFullscreen(bool fullscreen) {
#ifndef EMSCRIPTEN
        cout << "Fullscreen "<<fullscreen<<" value "<<(fullscreen ? SDL_WINDOW_FULLSCREEN : 0)<<endl;
        int error = SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
        if (error){
            cout << "Error "<<SDL_GetError()<<endl;
        }
        this->fullscreen = fullscreen;
#endif

    }

    void SDL2Context::setWindowTitle(std::string title) {
#ifndef EMSCRIPTEN
        SDL_SetWindowTitle(window, title.c_str());
#endif
    }

    std::string SDL2Context::getWindowTitle() {
#ifndef EMSCRIPTEN
        string title{SDL_GetWindowTitle(window)};
        return title;
#else
        return "";
#endif
    }
}