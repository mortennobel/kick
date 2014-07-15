//
// Created by morten on 15/07/14.
//

#include "log.h"
#include <iostream>

using namespace std;

namespace kick {
    namespace {
        void print(std::string type, std::string message, std::string func, std::string file, int line, ostream& out = cout) {
            out << type << " in "<<file<<":"<< line<<" in "<< func <<"()\n";
            out << message << endl;
        }
    }

    std::function<void (std::string, std::string, std::string, int)> Log::info = [](std::string message, std::string func, std::string file, int line){
        print("info", message, func, file, line);
    };
    std::function<void (std::string, std::string, std::string, int)> Log::warning = [](std::string message, std::string func, std::string file, int line){
        print("warning", message, func, file, line);
    };
    std::function<void (std::string, std::string, std::string, int)> Log::error = [](std::string message, std::string func, std::string file, int line){
        print("error", message, func, file, line, cerr);
    };

    void Log::disable() {
        function<void (std::string, std::string, std::string, int)> voidFn = [](std::string message, std::string func, std::string file, int line){};
        info = voidFn;
        warning = voidFn;
        error = voidFn;
    }

    void Log::reset() {
        info = [](std::string message, std::string func, std::string file, int line){
            print("info", message, func, file, line);
        };
        warning = [](std::string message, std::string func, std::string file, int line){
            print("warning", message, func, file, line);
        };
        error = [](std::string message, std::string func, std::string file, int line){
            print("error", message, func, file, line, cerr);
        };
    }
}