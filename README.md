kick
====

Simple game engine for C++ 11 / OpenGL 3.x +.

## Dependencies

 - SDL 2.x for platform abstraction layer
 - SDL Image 2.x for loading textures
 - GLM  0.9.x (OpenGL Mathematics). Header only library. Included in the build.
 - RapidJSON for JSON parsing. Header only library. Included in the build.
 - TinyTest. Header only library. Included in the build. Only used for unit-testing.

## Build
Using premake4 for creating make files or other project files. For more info see:
http://industriousone.com/premake

## Online docs
http://mortennobel.github.io/kick/

## Example usage
    #include "kick/kick.h"
    
    using namespace kick;
    
    int main(int argc, char * argv[])
    {
        Engine engine(argc, argv);
        auto scene = engine.getActiveScene();
        auto camera = scene->createPerspectiveCamera();
        camera->getGameObject()->getTransform()->setLocalPosition({0,0,10});
        scene->createSphere();
        scene->createDirectionalLight();
        engine.startMainLoop();
    
        return 0;
    }
