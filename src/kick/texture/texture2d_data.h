//
// Created by Morten Nobel-Jørgensen on 19/04/15.
//


#pragma once
#include <vector>
#include "glm/glm.hpp"
#include <cstdint>
#include <string>

namespace kick{

    class Texture2DData {
        std::vector<char> mData;
        int mWidth;
        int mHeight;
    public:
        Texture2DData(const std::vector<char>& data, int width, int height);

        const char * data() const;

        int width() const;

        int height() const;


        uint8_t& vec1uc(int x, int y);

        uint8_t* vec2uc(int x, int y);

        uint8_t* vec3uc(int x, int y);

        uint8_t* vec4uc(int x, int y);

        float& vec1(int x,int y);

        glm::vec2& vec2(int x,int y);

        glm::vec3& vec3(int x,int y);

        glm::vec4& vec4(int x,int y);

        void saveBMPImage(std::string filename, bool flipY = true);
    };
}



