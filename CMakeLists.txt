# Required for statically linking the MSVC runtime
set(CMAKE_USER_MAKE_RULES_OVERRIDE
   ${CMAKE_CURRENT_SOURCE_DIR}/cmake/c_flag_overrides.cmake)
set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX
   ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cxx_flag_overrides.cmake)

cmake_minimum_required  (VERSION 2.8)

set(app_name SimpleRenderer)
project(${app_name})
include(GenerateExportHeader)
include(cmake/defaults.cmake)


#the FILE GLOB commands create lists of files according to the regex and then
#put them into the variable
#it is like set(Sources but using patterns
FILE(GLOB LIBPNG_SOURCE source/renderlib/libpng/*.c)
FILE(GLOB LIBPNG_HEADERS source/renderlib/libpng/*.h)
FILE(GLOB SOIL_SOURCE source/renderlib/soil/*.c)
FILE(GLOB SOIL_HEADERS source/renderlib/soil/*.h)
FILE(GLOB RENDERLIB_SOURCE source/renderlib/*.*)
#FILE(GLOB RENDERLIB_HEADERS source/renderlib/*.h*)
FILE(GLOB MAIN_SOURCE source/*.*)
#FILE(GLOB MAIN_HEADERS source/*.h*)
FILE(GLOB PLATFORM_SOURCE source/PlatformClasses/*.*)
#FILE(GLOB PLATFORM_HEADERS source/PlatformClasses/*.h*)
FILE(GLOB SHADERS shaders/*.*)


set(SOURCES
   main.cpp
   ${RENDERLIB_SOURCE}
   ${MAIN_SOURCE}
   ${PLATFORM_SOURCE}
   ${LIBPNG_SOURCE}
   ${SOIL_SOURCE}
)


set(HEADERS
  source/renderlib/string_util/win32_debug_buffer.h
  ${LIBPNG_HEADERS}
  ${SOIL_HEADERS}
  ${OVR_HEADERS}
)


set(RESOURCES
)

set(ASSETS
   )

###############################################################################
###############################################################################
###############################################################################
#
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

string( TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_LOWER )
if(BUILD_TYPE_LOWER STREQUAL "debug")
   set(DEBUG 1)
endif()



###############################################################################
#
#
# GLEW - Cross platform access to OpenGL 3+ functionality
if(NOT APPLE)
add_definitions(-DGLEW_STATIC)
add_subdirectory(libraries/glew)
set_target_properties(glew PROPERTIES FOLDER "3rdparty")
list(APPEND EXTRA_LIBS glew)
endif(APPLE)

# GLFW - Cross platform OpenGL window creation and input handling
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target")
add_subdirectory(libraries/glfw)
set_target_properties(glfw PROPERTIES FOLDER "3rdparty")
list(APPEND EXTRA_LIBS glfw ${GLFW_LIBRARIES})
MESSAGE(GLFW LIBRARIES)
MESSAGE(${GLFW_LIBRARIES})

find_package(Threads)
list(APPEND EXTRA_LIBS ${CMAKE_THREAD_LIBS_INIT} )

include_directories(${CMAKE_SOURCE_DIR}/libraries/glfw/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/glew/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/glm)

###############################################################################
#
# GLM - Vector / matrix header only math library based on the syntax of GLSL
#
# It's cleverly puts all it's compiler and C++ feature  detection code into a
# header that switches based on what compiler is being used (based on predefined 
# macros).  Why can't everyone do that?
include_directories(${CMAKE_SOURCE_DIR}/libraries/glm)


###############################################################################
# ASSIMP - Cross platform 3D model import library
add_subdirectory(libraries/assimp)
set_target_properties(assimp PROPERTIES FOLDER "3rdparty")
list(APPEND EXTRA_LIBS assimp)

include_directories(${CMAKE_SOURCE_DIR}/libraries/assimp/include)

set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "If the supplementary tools for Assimp are built in addition to the library.")
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "If the test suite for Assimp is built in addition to the library..")
###############################################################################
# SOIL - Cross platform texture import library

include_directories(${CMAKE_SOURCE_DIR}/source/renderlib/soil)
###############################################################################
###############################################################################


#creates folders in the IDE's project explorer.  Better than everything
#showing up in SOURCE FILES
source_group("soil" FILES ${SOIL_SOURCE} ${SOIL_HEADERS})
source_group("libpng" FILES ${LIBPNG_SOURCE} ${LIBPNG_HEADERS})
source_group("md5" FILES source/md5.cpp source/md5.h)

# -g adds debugging symbols
# -Wall turns on all warnings
# -Wextra turns on a lot of warnings (but not too pedantic)
if(APPLE)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wextra -Wno-ignored-qualifiers -Wno-unused-parameter -std=c++11")
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -g -Wextra -Wno-ignored-qualifiers -Wno-unused-parameter")
  add_definitions(-DTARGETMAC)
  include_directories(source/PlatformClasses/Apple/MacClasses)
  LIST(APPEND SOURCES source/PlatformClasses/Apple/MacClasses/Platform_Mac.mm)
  LIST(APPEND HEADERS source/PlatformClasses/Apple/MacClasses/Platform_Mac.h)

#  link_directories(${CMAKE_CURRENT_SOURCE_DIR}/GLFW) #had to put here because the libs can conflict on linux
ENDIF(APPLE)

if(WIN32)
#  include_directories(glew/include)
  LIST(APPEND SOURCES source/PlatformClasses/Windows/Platform_Win32.cpp)
  LIST(APPEND HEADERS source/PlatformClasses/Windows/Platform_Win32.h)
  include_directories(source/PlatformClasses/Windows)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  add_definitions(-D_CRT_SECURE_NO_WARNINGS) #disable sprintf deprecation
#  link_directories(${CMAKE_CURRENT_SOURCE_DIR}/GLFW) #had to put here because the libs can conflict on linux
ENDIF(WIN32)

#SET(CMAKE_OSX_ARCHITECTURES "i386")


if(CMAKE_BUILD_TYPE MATCHES DEBUG)
    message("adding debug flags")
    add_definitions(-DDEBUG)
    add_definitions(-D_DEBUG)
    add_definitions(-DBREAK_ON_EXCEPTION)
endif(CMAKE_BUILD_TYPE MATCHES DEBUG)



#add_subdirectory(renderlib) #requires the directory to have a CMakeLists.txt file
include_directories(source)
include_directories(source/renderlib)
include_directories(source/renderlib)
include_directories(source/PlatformClasses)
include_directories(ovr_code)


if(APPLE)
   FIND_LIBRARY(IOKIT_LIBRARY IOKit)
   FIND_LIBRARY(COCOA_LIBRARY Cocoa)
   FIND_LIBRARY(OPENGL_LIBRARY OpenGL)
   FIND_LIBRARY(CORE_MIDI_LIBRARY CoreMidi)

   MARK_AS_ADVANCED(
                     IOKIT_LIBRARY
                     COCOA_LIBRARY
                     OPENGL_LIBRARY
                     CORE_MIDI_LIBRARY
                   )
  LIST(
      APPEND
      EXTRA_LIBS
      ${IOKIT_LIBRARY}
      ${COCOA_LIBRARY}
      ${OPENGL_LIBRARY}
      ${CORE_MIDI_LIBRARY}
#libglfw3.a
      )
ENDIF(APPLE)

if(WIN32)
  find_package(OpenGL REQUIRED)
  include_directories(${OpenGL_INCLUDE_DIRS})
  link_directories(${OpenGL_LIBRARY_DIRS})
  add_definitions(${OpenGL_DEFINITIONS})
  if(NOT OPENGL_FOUND)
      message(ERROR " OPENGL not found!")
  endif(NOT OPENGL_FOUND)

#cmake does not like if you add .lib to the libraries on windows!
  LIST(
      APPEND
      EXTRA_LIBS
      winmm
      )
ENDIF(WIN32)

if(UNIX AND NOT APPLE)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wextra -Wno-ignored-qualifiers -Wno-unused-parameter -std=c++11")
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -g -Wextra -Wno-ignored-qualifiers -Wno-unused-parameter")
  add_definitions(-DTARGEUNIX)
  include_directories(libraries/glew/include)
  LIST(APPEND SOURCES libraries/glew/glew.c)
  LIST(APPEND SOURCES source/PlatformClasses/Unix/Platform_Unix.cpp)
  LIST(APPEND HEADERS source/PlatformClasses/Unix/Platform_Unix.h)
  include_directories(source/PlatformClasses/Unix)

  
#find_package(PkgConfig REQUIRED)
#  pkg_search_module(GLFW REQUIRED glfw3)
#  include_directories(${GLFW_INCLUDE_DIRS})
#  SET(EXTRA_LIBS ${GLFW_STATIC_LIBRARIES})

ENDIF(UNIX AND NOT APPLE)

#required when using GLEW as a static library
add_definitions(-DGLEW_STATIC)

#Setting up target and it's properties
if(APPLE)
#CREATING A BUNDLE SEEMS TO DISABLE RETINA SUPPORT FOR GLFW!!!!
#  add_executable(${app_name} MACOSX_BUNDLE ${SOURCES} ${HEADERS} ${SHADERS} ${ASSETS} ${RESOURCES})
  add_executable(${app_name} ${SOURCES} ${HEADERS} ${SHADERS} ${ASSETS} ${RESOURCES})
#this is where to put the dylibs
  SET_SOURCE_FILES_PROPERTIES(

      PROPERTIES
      MACOSX_PACKAGE_LOCATION MacOSX
      )
#you have to mark each file as a resource
  foreach( rez ${RESOURCES} )
    message(${rez})
    SET_TARGET_PROPERTIES(${APP_NAME} PROPERTIES RESOURCE "${rez}")
  endforeach( rez )
  message("RESOURCES: " ${RESOURCES})

#  foreach( shader ${SHADERS} )
#    message(${shader})
#    SET_TARGET_PROPERTIES(${APP_NAME} PROPERTIES RESOURCE "${shader}")
#  endforeach( shader )
#  message("Shaders: " ${SHADERS})

  foreach( asset ${ASSETS} )
    message(${asset})
    SET_TARGET_PROPERTIES(${APP_NAME} PROPERTIES RESOURCE "${asset}")
  endforeach( asset )

#this is where to put the resources
  message("Resources: " ${RESOURCES})
  SET_SOURCE_FILES_PROPERTIES(
      ${RESOURCES}
      PROPERTIES
      MACOSX_PACKAGE_LOCATION Resources
      )
  message("Assets: " ${ASSETS})
  SET_SOURCE_FILES_PROPERTIES(
      ${ASSETS}
      PROPERTIES
      MACOSX_PACKAGE_LOCATION Resources/assets
      )
#  SET_SOURCE_FILES_PROPERTIES(
#      ${SHADERS}
#      PROPERTIES
#      MACOSX_PACKAGE_LOCATION Resources/Shaders
#      )
else()
  add_executable(${app_name} ${SOURCES} ${HEADERS} ${SHADERS} ${RESOURCES})
endif(APPLE)

message("EXTRA LIBS " ${EXTRA_LIBS})
target_link_libraries(${app_name} ${EXTRA_LIBS})
