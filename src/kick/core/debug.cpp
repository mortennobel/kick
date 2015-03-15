//
// Created by morten on 15/07/14.
//

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include "debug.h"
#include <iostream>
#ifdef __APPLE__
#include <execinfo.h>
#endif
#include <stdio.h>  /* defines FILENAME_MAX */


#include "glm/glm.hpp"

using namespace std;

namespace kick {
    namespace {
        void print(std::string type, std::string message, std::string func, std::string file, int line, ostream& out = cout) {
            out << type << " in "<<file<<":"<< line<<" in "<< func <<"()\n";
            out << message << endl;
        }
    }

    std::function<void (std::string, std::string, std::string, int)> Debug::info = [](std::string message, std::string func, std::string file, int line){
        print("info", message, func, file, line);
    };
    std::function<void (std::string, std::string, std::string, int)> Debug::warning = [](std::string message, std::string func, std::string file, int line){
        print("warning", message, func, file, line);
    };
    std::function<void (std::string, std::string, std::string, int)> Debug::error = [](std::string message, std::string func, std::string file, int line){
        print("error", message, func, file, line, cerr);
    };

    void Debug::disable() {
        function<void (std::string, std::string, std::string, int)> voidFn = [](std::string message, std::string func, std::string file, int line){};
        info = voidFn;
        warning = voidFn;
        error = voidFn;
    }

    void Debug::reset() {
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

	string Debug::workingDir(){
		char cCurrentPath[FILENAME_MAX];

#ifdef _WIN32
		if (!GetCurrentDirectoryA(FILENAME_MAX, cCurrentPath))
#else
		if (!getcwd(cCurrentPath, sizeof(cCurrentPath)))
#endif
		{
			return "Error";
		}

		return std::string(cCurrentPath);
	}

    // based on http://oroboro.com/stack-trace-on-crash/
    void Debug::printStacktrace(){
#ifdef __APPLE__
        FILE *out = stderr;
        unsigned int max_frames = 63;
        fprintf(out, "stack trace:\n");

        // storage array for stack trace address data
        void* addrlist[max_frames+1];

        // retrieve current stack addresses
        glm::u32 addrlen = backtrace( addrlist, sizeof( addrlist ) / sizeof( void* ));

        if ( addrlen == 0 )
        {
            fprintf( out, "  \n" );
            return;
        }

        // create readable strings to each frame.
        char** symbollist = backtrace_symbols( addrlist, addrlen );

        // print the stack trace.
        for ( glm::u32 i = 4; i < addrlen; i++ )
            fprintf( out, "%s\n", symbollist[i]);

        free(symbollist);

#endif
    }

    void Debug::drawLine(glm::vec3 from, glm::vec3 to, float seconds, glm::vec4 color, Camera* camera) {
        cout << "Implement draw line"<<endl; // todo
    }

    void Debug::drawSphere(glm::vec3 center, float size, float seconds, glm::vec4 color, Camera* camera) {
        cout << "Implement draw sphere"<<endl; // todo
    }

    void Debug::drawBox(glm::vec3 center, glm::vec3 size, float seconds, glm::vec4 color, Camera* camera) {
        cout << "Implement draw box"<<endl; // todo
    }
}