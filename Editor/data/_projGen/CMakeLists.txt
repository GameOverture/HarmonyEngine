cmake_minimum_required(VERSION 3.10)
project(%HY_CLASS% VERSION 1.0)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(HYBUILD_DebugEmscripten "Use a Debug build configuration when targeting web browsers" OFF)

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

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
	message(STATUS "Building for ${CMAKE_SYSTEM_NAME} target system")
	set(EM_LINK_FLAGS "-s ALLOW_MEMORY_GROWTH=1 -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -s USE_SDL=2 --no-heap-copy --preload-file %HY_PROJDIR%%HY_RELDATADIR%@%HY_RELDATADIR% --preload-file %HY_PROJDIR%%HY_CLASS%.hyproj@%HY_CLASS%.hyproj")
	if(HYBUILD_DebugEmscripten)
		message(STATUS "Building for DEBUG")
		set(CMAKE_BUILD_TYPE Debug)
		target_compile_definitions(HyEngine PRIVATE DEBUG)
		target_compile_definitions(%HY_CLASS% PRIVATE DEBUG)
		set_target_properties(%HY_CLASS% PROPERTIES COMPILE_FLAGS_FLAGS "-fsanitize=undefined -g")
		set(EM_LINK_FLAGS "${EM_LINK_FLAGS} -g -s ASSERTIONS=1 -s SAFE_HEAP=1 -s STACK_OVERFLOW_CHECK=2 -s LLD_REPORT_UNDEFINED")
	endif()
	set_target_properties(%HY_CLASS% PROPERTIES LINK_FLAGS ${EM_LINK_FLAGS})
	set(CMAKE_EXECUTABLE_SUFFIX ".html")
else()
	target_include_directories(%HY_CLASS% PUBLIC "%HY_HARMONYDIR%Engine/external/glad/include")
	target_include_directories(%HY_CLASS% PUBLIC "%HY_HARMONYDIR%Engine/external/glfw/include")
	target_precompile_headers(%HY_CLASS% PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/%HY_SRCDIR%/pch.h")
endif()

target_link_libraries(%HY_CLASS% PUBLIC HyEngine)

if(TARGET uninstall)
	set_target_properties(uninstall PROPERTIES FOLDER "Engine Libs")
endif()

if (MSVC)
	set_property(TARGET %HY_CLASS% PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
	set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT %HY_CLASS%)
endif()
