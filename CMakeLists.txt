cmake_minimum_required(VERSION 3.10)
project(Harmony)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(HYBUILD_Qt ON)

# Build Extras
option(HYBUILD_SPINE "Runtime support for Spine 2d animation - requires Spine License" OFF)
option(HYBUILD_RAPIDJSON "Replaces default JSON library (nholmann) with a provided RapidJSON" OFF)

if(HYBUILD_SPINE)
	add_compile_definitions(HY_USE_SPINE)
endif()
if(HYBUILD_RAPIDJSON)
	add_compile_definitions(HY_USE_RAPIDJSON)
endif()

add_subdirectory(Engine)
add_subdirectory(Editor)

# Build engine as GUI/Editor mode
target_compile_definitions(HyEngine PRIVATE HY_USE_QT)

# Default startup project for Visual Studio will be the editor
if (MSVC)
	set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT HyEditor)
endif()