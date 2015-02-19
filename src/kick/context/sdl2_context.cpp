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
#ifndef EMSCRIPTEN
#ifdef __APPLE__
#include <SDL2_image/SDL_image.h>
#else
#include <SDL2_image/SDL_image.h>
#endif
#endif

using namespace std;
using namespace glm;

namespace kick {


    SDL2Context::SDL2Context(){
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
        if (glContext){
            SDL_GL_DeleteContext(glContext);
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
#endif
        bool res = SDL_Init(SDL_INIT_EVERYTHING) >= 0;

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
        

        if (depthSize>0){
            /* Turn on double buffering with a 24bit Z buffer.
             * You may need to change this to 16 or 32 for your system */
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depthSize);
        }


#ifdef EMSCRIPTEN
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
        if (config.multisamples>1) {
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, config.multisamples);
        }
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
#endif

        /* Create our window centered at 512x512 resolution */
        window = SDL_CreateWindow(windowName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

        if (!window)
            return false;

#ifndef EMSCRIPTEN
        SDL_Surface * iconSurface = IMG_Load("assets/ui/icon.png");
        if (iconSurface){
            // The icon is attached to the window pointer
            SDL_SetWindowIcon(window, iconSurface);
            SDL_FreeSurface(iconSurface);
        }
#endif

        glContext = SDL_GL_CreateContext(window);

#ifdef _WIN32
		//Initialize GLEW
		glewExperimental = GL_TRUE;
		GLenum glewError = glewInit();
		if (glewError != GLEW_OK)
		{
			printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
		}
#endif

        if (!glContext)
            return false;

        int w,h;
        SDL_GL_GetDrawableSize(window, &w, &h);
        contextSurfaceDim = glm::ivec2(w, h);

#endif
        return true;
    }
    
    void SDL2Context::swapBuffer(){
        SDL_GL_SwapWindow(window);
    }
    
    void SDL2Context::step(){
        Engine::update();
        Engine::render();
    }

    bool SDL2Context::tick(){
#ifdef EMSCRIPTEN
        // ugly hack - checks size each frame (since no events is fired)
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        if (contextSurfaceDim.x != w || contextSurfaceDim.y != h){
            contextSurfaceDim.x = w;
            contextSurfaceDim.y = h;
            contextSurfaceSize.notifyListeners(contextSurfaceDim);
        }
#endif
        bool quit = false;
        SDL_Event event;
        Engine::startFrame();
        while(SDL_PollEvent(&event))
        {
            switch (event.type) {
                case SDL_QUIT:
                    quit = 1;
                    break;
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED || event.window.event == SDL_WINDOWEVENT_RESIZED ){
                        if (contextSurfaceDim.x != event.window.data1 || contextSurfaceDim.y != event.window.data2){
                            contextSurfaceDim.x = event.window.data1;
                            contextSurfaceDim.y = event.window.data2;
                            contextSurfaceSize.notifyListeners(contextSurfaceDim);
                        }
                    }
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
                case SDL_FINGERDOWN: /* Touch events */
                    handleTouch(event.tfinger);
                    break;
                case SDL_FINGERUP:
                    handleTouch(event.tfinger);
                    break;
                case SDL_FINGERMOTION:
                    handleTouch(event.tfinger);
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

#endif
                default:
                    // unhandled
                    cout << "Unhandled event "<< event.type << endl;
                    break;
            }
        }
        step();
        return quit;
    }

#ifdef EMSCRIPTEN
    void kick_emscripten_tick(){
        SDL2Context *c = (SDL2Context*)Engine::context();
        c->tick();
    }
#endif

    void SDL2Context::mainLoop(){
#ifdef EMSCRIPTEN
        int fps = 0; // 0 is RequestAnimFrame
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
            KeyInput::pressBegin(key);
        } else {
            KeyInput::pressEnd(key);
        }
    }
    void SDL2Context::handleMouseMotion(SDL_MouseMotionEvent event){
        MouseInput::setPosition(ivec2(event.x,event.y));
        MouseInput::setPositionDelta(ivec2(event.xrel,event.yrel));
    }
    
    void SDL2Context::handleMouseButton(SDL_MouseButtonEvent event, bool buttonDown){
        if (buttonDown){
            MouseInput::buttonPressStarted(event.button-1);
        } else {
            MouseInput::buttonPressEnded(event.button-1);
        }
#ifndef EMSCRIPTEN
        MouseInput::setClicks(event.clicks);
#endif
    }
    
    void  SDL2Context::handleMouseWheel(SDL_MouseWheelEvent event){
        MouseInput::setMouseWheelDelta(ivec2(event.x, event.y));
    }

    bool SDL2Context::isFullscreen() {
        return fullscreen;
    }

    void SDL2Context::setFullscreen(bool fullscreen) {
#ifndef EMSCRIPTEN
        /*int error = SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
        if (error){
            cout << "Error "<<SDL_GetError()<<endl;
        }
        this->fullscreen = fullscreen;
          */
        if (this->fullscreen != fullscreen){
            Uint32 flags = (SDL_GetWindowFlags(window) ^ SDL_WINDOW_FULLSCREEN_DESKTOP);
            if (SDL_SetWindowFullscreen(window, flags) < 0) // NOTE: this takes FLAGS as the second param, NOT true/false!
            {
                std::cout << "Toggling fullscreen mode failed: " << SDL_GetError() << std::endl;
                return;
            }

            /*Engine::getEventQueue().scheduleEvent([&](int eventid){
                SDL_GL_GetDrawableSize(window, &contextSurfaceDim.x, &contextSurfaceDim.y);
                contextSurfaceSize.notifyListeners(contextSurfaceDim);
            });*/

            this->fullscreen = fullscreen;
        }
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

    void SDL2Context::handleTouch(SDL_TouchFingerEvent event) {
        ivec2 pos{(int)round(contextSurfaceDim.x * event.x), (int)round(contextSurfaceDim.y * event.y)};
        switch (event.type){
            case SDL_FINGERDOWN:
                TouchInput::setTouchStarted(event.fingerId, pos, event.pressure);
                break;
            case SDL_FINGERMOTION:
                TouchInput::setTouchMoved(event.fingerId, pos, event.pressure);
                break;
            case SDL_FINGERUP:
                TouchInput::setTouchEnded(event.fingerId, pos);
                break;
            default:
                cout << "Unknown event type "<<event.type<<endl;
        }
    }

}