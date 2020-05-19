cmake_minimum_required(VERSION 3.10)
project(%HY_CLASS% VERSION 1.0)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(HYBUILD_Emscripten "Build game for web browsers" OFF)

add_subdirectory("%HY_HARMONYDIR%Engine" "Engine")

set(HY_SRCFILES
	"%HY_SRCDIR%/pch.h"
	"%HY_SRCDIR%/main.cpp"
	"%HY_SRCDIR%/%HY_CLASS%.cpp"
	"%HY_SRCDIR%/%HY_CLASS%.h"
	"%HY_CLASS%.hyproj")

add_executable(%HY_CLASS% ${HY_SRCFILES})
set_target_properties(%HY_CLASS% PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO)

target_include_directories(%HY_CLASS% PUBLIC "%HY_HARMONYDIR%Engine/include")
target_include_directories(%HY_CLASS% PUBLIC "%HY_HARMONYDIR%Engine/external/box2d/include")
target_include_directories(%HY_CLASS% PUBLIC "%HY_HARMONYDIR%Engine/external/soil2/include")

if(HYBUILD_Emscripten)
	if(CMAKE_BUILD_TYPE EQUAL "debug")
		target_compile_definitions(HyEngine PUBLIC DEBUG)
		target_compile_definitions(%HY_CLASS% PUBLIC DEBUG)
		set_target_properties(%HY_CLASS% PROPERTIES COMPILE_FLAGS_FLAGS "-fsanitize=undefined -g")
		set_target_properties(%HY_CLASS% PROPERTIES LINK_FLAGS "-g -s ASSERTIONS=1 -s SAFE_HEAP=1 -s STACK_OVERFLOW_CHECK=2 -s LLD_REPORT_UNDEFINED")
	endif()
	set_target_properties(%HY_CLASS% PROPERTIES LINK_FLAGS "-s ALLOW_MEMORY_GROWTH=1 -s USE_GLFW=3 -s MAX_WEBGL_VERSION=2 --no-heap-copy --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/%HY_CLASS%.hyproj@%HY_CLASS%.hyproj --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/%HY_DATADIR%")
	set(CMAKE_EXECUTABLE_SUFFIX ".html")
else()
	target_include_directories(%HY_CLASS% PUBLIC "%HY_HARMONYDIR%Engine/external/glad/include")
	target_include_directories(%HY_CLASS% PUBLIC "%HY_HARMONYDIR%Engine/external/glfw/include")
	target_precompile_headers(%HY_CLASS% PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/%HY_SRCDIR%/pch.h")
endif()

target_link_libraries(%HY_CLASS% PUBLIC HyEngine)

if (MSVC)
	set_property(TARGET %HY_CLASS% PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
	set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT %HY_CLASS%)
endif()
