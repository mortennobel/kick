//
// Created by morten on 15/07/14.
//

#include "log.h"
#include <iostream>

using namespace std;

namespace kick {
    namespace {
        void print(std::string type, std::string message, std::string func, std::string file, int line) {
            cout << type << " in "<<file<<":"<< line<<"\n";
            cout << message << endl;
        }
    }

    void Log::info(std::string message, std::string func, std::string file, int line) {
        print("info", message, func, file, line);
    }

    void Log::warning(std::string message,std::string func,  std::string file, int line) {
        print("warning", message, func, file, line);
    }

    void Log::error(std::string message, std::string func, std::string file, int line) {
        print("error", message, func, file, line);
    }
}