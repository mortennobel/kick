solution "kick"
  configurations { "Release", "Debug" }

project "kick"
  kind     "StaticLib"
  language "C++"
  files    { "src/**.h","src/**.cpp","src/**.inl", "assets/**" }
  includedirs { "src", "libs/include" }
  configuration {"xcode*" }
    buildoptions "-std=c++1y -stdlib=libc++ -fno-exceptions"
  configuration {"xcode*" }
    links { "Cocoa.framework", "OpenGL.framework",  "/Library/Frameworks/SDL2.framework", "/Library/Frameworks/SDL2_image.framework"  }
  configuration {"xcode*" }
    buildoptions { "-F/Library/Frameworks/" }
  
  
  configuration "Debug"
    targetdir "lib/debug"
    defines { "DEBUG" }
    flags { "Symbols" }		

  configuration "Release"
    targetdir "lib/release"
    defines { "NDEBUG" }
    flags { "Optimize" } 	
	
  if _ACTION == "clean" then
    os.rmdir("lib")
  end
  
project "kick_unittest"
  kind "ConsoleApp"
  language "C++"
  files    { "src/**.h","src/**.cpp", "src/**.inl", "unittest/**.cpp","unittest/**.h", "assets/**" }
  includedirs { "src", "unittest", "libs/include" }
  buildoptions "-std=c++1y -stdlib=libc++ -fno-exceptions"
  links { "Cocoa.framework", "GLUT.framework", "OpenGL.framework", "/Library/Frameworks/SDL2.framework", "/Library/Frameworks/SDL2_image.framework" }

  buildoptions { "-F/Library/Frameworks/" }
  linkoptions { "-F/Library/Frameworks/"}
  configuration "Debug"
    targetdir "bin/debug"
    defines { "DEBUG" }
    flags { "Symbols" }		
    debugdir "../.."

  configuration "Release"
    targetdir "bin/release"
    defines { "NDEBUG" }
    flags { "Optimize" } 	
    debugdir "../.."
	
  if _ACTION == "clean" then
    os.rmdir("bin")
  end
