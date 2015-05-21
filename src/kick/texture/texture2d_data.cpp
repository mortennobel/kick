//
// Created by Morten Nobel-Jørgensen on 19/04/15.
//

#include "texture2d_data.h"

#ifndef EMSCRIPTEN
#ifdef _WIN32
#include <SDL_surface.h>
#else
#include <SDL2/SDL_surface.h>
#endif
#endif

kick::Texture2DData::Texture2DData(const std::vector<char>& data, int width, int height)
        : mData(data), mWidth(width), mHeight(height) {
}

const char *kick::Texture2DData::data() const {
    return mData.data();
}

int kick::Texture2DData::height() const {
    return mHeight;
}

int kick::Texture2DData::width() const {
    return mWidth;
}

glm::vec4 &kick::Texture2DData::vec4(int x, int y) {
    glm::vec4* ptr = reinterpret_cast<glm::vec4*>(mData.data());
    return ptr[y* width() +  x];
}

glm::vec3 &kick::Texture2DData::vec3(int x, int y) {
    glm::vec3* ptr = reinterpret_cast<glm::vec3*>(mData.data());
    return ptr[y* width() +  x];
}

glm::vec2 &kick::Texture2DData::vec2(int x, int y) {
    glm::vec2* ptr = reinterpret_cast<glm::vec2*>(mData.data());
    return ptr[y* width() +  x];
}

float &kick::Texture2DData::vec1(int x, int y) {
    float* ptr = reinterpret_cast<float*>(mData.data());
    return ptr[y* width() +  x];
}

uint8_t *kick::Texture2DData::vec4uc(int x, int y) {
    uint8_t* ptr = reinterpret_cast<uint8_t*>(mData.data());
    return ptr+ (y*width() +  x)*4;
}

uint8_t *kick::Texture2DData::vec3uc(int x, int y) {
    uint8_t* ptr = reinterpret_cast<uint8_t*>(mData.data());
    return ptr+ (y*width() +  x)*3;
}

uint8_t *kick::Texture2DData::vec2uc(int x, int y) {
    uint8_t* ptr = reinterpret_cast<uint8_t*>(mData.data());
    return ptr+ (y*width() +  x)*2;
}

uint8_t &kick::Texture2DData::vec1uc(int x, int y) {
    uint8_t* ptr = reinterpret_cast<uint8_t*>(mData.data());
    return ptr[y*width() +  x];
}

void kick::Texture2DData::saveBMPImage(std::string filename,bool flipY) {
#ifndef EMSCRIPTEN
    std::vector<char> copy(mData.size());
    if (flipY){
        for (int y=0;y<mHeight;y++){
            for (int x=0;x<mWidth*4;x++){
                copy[(mHeight-y-1)*mWidth*4 + x] = mData[y*mWidth*4 + x];
            }
        }
    } else{
        copy = mData;
    }
    SDL_Surface* sdlSurface = SDL_CreateRGBSurfaceFrom(copy.data(), mWidth, mHeight,32, mWidth *4, 0xff,0xff<<8,0xff<<16,0xff<<24);

    SDL_SaveBMP(sdlSurface, filename.c_str());
    sdlSurface->pixels = nullptr;
    SDL_FreeSurface(sdlSurface);
#endif
}
