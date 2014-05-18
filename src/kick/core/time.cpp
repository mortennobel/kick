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
    float Time::getTime() {
        static time_point<system_clock> start = system_clock::now();
        auto elapsed_seconds = duration_cast<seconds>(system_clock::now()-start);
        return elapsed_seconds.count();
    }

    long Time::getFrame() {
        return frame;
    }
};