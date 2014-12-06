//
// Created by morten on 06/12/14.
//

#include "random.h"

namespace kick {
    Random::Random() : e1(rd()) {
}

float Random::nextFloat(float min, float maxExcluding) {
    float r = next();
    while (r == 1.0f){
        r = next();
    }
    return r*(maxExcluding-min) + min;
}

int Random::nextInt(int min, int max) {
    return (int)floor(nextFloat(min, max));
}

float Random::next(){
    return std::generate_canonical<float, 32>(e1);;
}

    void Random::seed(unsigned int s){
        e1.seed(s);
    }
}