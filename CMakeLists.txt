cmake_minimum_required(VERSION 3.2)

project(kickproject C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/libs/include)

#file(GLOB_RECURSE SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)
set(SOURCE_FILES
   ${CMAKE_SOURCE_DIR}/src/kick/2d/button.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/2d/canvas.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/2d/component2d.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/2d/font.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/2d/label.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/2d/sprite.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/2d/sprite_mouse_listener.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/2d/toggle_button.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/context/context.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/context/nativedialog.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/context/sdl2_context.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/context/window_config.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/core/debug.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/core/default_key_handler.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/core/engine.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/core/event_listener.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/core/event_queue.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/core/key_input.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/core/kickgl.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/core/mouse_input.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/core/project.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/core/project_asset.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/core/time.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/core/touch_input.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/material/material.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/material/shader.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/material/shader_enums.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/math/aabb.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/math/bounds2.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/math/bounds3.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/math/frustum.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/math/glm_ext.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/math/kd_tree.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/math/misc.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/math/plane.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/math/random.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/math/ray.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/math/spherical.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/mesh/mesh.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/mesh/mesh_data.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/mesh/mesh_factory.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/scene/camera.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/scene/camera_orthographic.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/scene/camera_perspective.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/scene/component.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/scene/engine_uniforms.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/scene/game_object.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/scene/light.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/scene/line_renderer.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/scene/mesh_renderer.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/scene/scene.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/scene/scene_lights.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/scene/skybox.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/scene/transform.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/texture/image_format.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/texture/texture2d.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/texture/texture2d_data.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/texture/texture_atlas.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/texture/texture_cube.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/texture/texture_render_target.cpp
   ${CMAKE_SOURCE_DIR}/src/kick/texture/texture_sampler.cpp
   ${CMAKE_SOURCE_DIR}/libs/src/tinyfiledialogs.c
   )

set(UNITTEST_SOURCE_FILES ${CMAKE_SOURCE_DIR}/unittest/unittest.cpp)


add_library(kick ${SOURCE_FILES})
set_target_properties(kick PROPERTIES LINKER_LANGUAGE CXX)

set(SOURCE_AND_UNITTEST_FILES ${SOURCE_FILES} ${UNITTEST_SOURCE_FILES})

add_executable(kick_unittest ${SOURCE_AND_UNITTEST_FILES})

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

#########################################################
# FIND OPENGL
#########################################################
include_directories(${SDL2_INCLUDE_DIR})
IF(APPLE)
   INCLUDE_DIRECTORIES ( /System/Library/Frameworks )
   FIND_LIBRARY(OpenGL_LIBRARY OpenGL )
      MARK_AS_ADVANCED (OpenGL_LIBRARY)
      SET(EXTRA_LIBS ${OpenGL_LIBRARY})
ELSE(TRUE)
   find_package(OpenGL REQUIRED)
   include_directories(${OpenGL_INCLUDE_DIRS})
   SET(EXTRA_LIBS ${OPENGL_LIBRARY})
ENDIF (APPLE)

target_link_libraries(kick_unittest ${EXTRA_LIBS} ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARIES})


