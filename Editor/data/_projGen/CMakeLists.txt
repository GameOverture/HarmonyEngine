cmake_minimum_required(VERSION 3.10)

project(%HY_CLASS% VERSION 1.0)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory("%HY_HARMONYDIR%Engine" "Engine")

set(HY_SRCFILES
	"%HY_SRCDIR%/pch.h"
	"%HY_SRCDIR%/main.cpp"
	"%HY_SRCDIR%/%HY_CLASS%.cpp"
	"%HY_SRCDIR%/%HY_CLASS%.h"
	"%HY_CLASS%.hyproj")

add_executable(%HY_CLASS% ${HY_SRCFILES})
set_target_properties(%HY_CLASS% PROPERTIES
	CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

target_include_directories(%HY_CLASS% PUBLIC "%HY_HARMONYDIR%Engine/include")
target_link_libraries(%HY_CLASS% PUBLIC HyEngine)

set_property(TARGET %HY_CLASS% PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
set_target_properties(%HY_CLASS% PROPERTIES
	CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

target_precompile_headers(%HY_CLASS% PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/%HY_SRCDIR%/pch.h")

if (MSVC)
	set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT %HY_CLASS%)
endif()