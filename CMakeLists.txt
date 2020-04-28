cmake_minimum_required(VERSION 3.10)

# set the project name
project(Harmony VERSION 1.0)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(src/Engine)
add_subdirectory(src/Editor)

# default startup project for Visual Studio
if (MSVC)
	# set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT HyEditor)
endif()