//
// Created by morten on 06/12/14.
//


#pragma once
#include <random>

namespace kick{
class Random {
    std::random_device rd;
    std::default_random_engine e1;
public:
    Random();

    float nextFloat(float min, float maxExcluding);

    int nextInt(int min, int max);

    float next();

    void seed(unsigned int s);
};
}