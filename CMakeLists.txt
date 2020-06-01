cmake_minimum_required(VERSION 3.10)
project(Harmony VERSION 1.0)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(HYBUILD_Qt ON)

add_subdirectory(Engine)
add_subdirectory(Editor)

# Build engine as GUI/Editor mode
target_compile_definitions(HyEngine PRIVATE HY_USE_QT)

# Default startup project for Visual Studio will be the editor
if (MSVC)
	set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT HyEditor)
endif()