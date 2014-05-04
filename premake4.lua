solution "kick"
  configurations { "Release", "Debug" }

project "kick"
  kind     "StaticLib"
  language "C++"
  files    { "src/**.h","src/**.cpp","src/**.inl", "assets/**" }
  includedirs { "src", "libs/include" }
  buildoptions "-std=c++11 -stdlib=libc++"
  links { "Cocoa.framework", "OpenGL.framework",  "/Library/Frameworks/SDL2.framework", "/Library/Frameworks/SDL2_image.framework"  }
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
  buildoptions "-std=c++11 -stdlib=libc++"
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
