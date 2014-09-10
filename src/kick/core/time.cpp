//
// Created by morten on 18/05/14.
//

#import "time.h"
#include <chrono>
#include <ctime>

using namespace std;
using namespace std::chrono;

namespace kick {
    long Time::frame = 0;
    float Time::deltaTime = 0;
    float Time::getTime() {
        static time_point<system_clock> start = system_clock::now();
        auto elapsed_seconds = duration_cast<std::chrono::microseconds>(system_clock::now()-start);
        return elapsed_seconds.count()*1.0e-6f;
    }

    long Time::getFrame() {
        return frame;
    }

    float Time::getDeltaTime() {
        return deltaTime;
    }
};