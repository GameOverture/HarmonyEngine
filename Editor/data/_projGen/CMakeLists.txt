cmake_minimum_required(VERSION 3.10)
project(%HY_CLASS%)

set(REL_SRC_DIR "%HY_SRCDIR%")
set(REL_DATA_DIR "%HY_RELDATADIR%")
set(ABS_HARMONY_DIR "%HY_HARMONYDIR%")

set(GAME_SRCFILES
	"${REL_SRC_DIR}/pch.h"
	"${REL_SRC_DIR}/main.cpp"
	"${REL_SRC_DIR}/Game/${PROJECT_NAME}.cpp"
	"${REL_SRC_DIR}/Game/${PROJECT_NAME}.h")
	
set(GAME_INCLUDE_DIRS "")
list(APPEND GAME_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src")
list(APPEND GAME_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src/Game")

#################################################################################################
# ^ ^ ^ USER CONFIG ABOVE ^ ^ ^

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

option(HYBUILD_DebugEmscripten "Use a Debug build configuration when targeting web browsers" OFF)
option(HYBUILD_SDL_NET "Use SDL2_NET" ON)

add_subdirectory("${ABS_HARMONY_DIR}/Engine" "HyEngine")
%HY_DEPENDENCIES_ADD%
add_executable(${PROJECT_NAME} WIN32 ${GAME_SRCFILES} "${PROJECT_NAME}.hyproj")
set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO)
	
target_include_directories(${PROJECT_NAME} PRIVATE "$<BUILD_INTERFACE:${GAME_INCLUDE_DIRS}>")

message(STATUS "Building for ${CMAKE_SYSTEM_NAME} target system")
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
	set(EM_LINK_FLAGS "")
	list(APPEND EM_LINK_FLAGS "-s ALLOW_MEMORY_GROWTH=1")
	list(APPEND EM_LINK_FLAGS "-s MIN_WEBGL_VERSION=2")
	list(APPEND EM_LINK_FLAGS "-s MAX_WEBGL_VERSION=2")
	list(APPEND EM_LINK_FLAGS "-s USE_SDL=2")
	list(APPEND EM_LINK_FLAGS "-s USE_OGG=1")
	list(APPEND EM_LINK_FLAGS "-s USE_VORBIS=1")
	list(APPEND EM_LINK_FLAGS "-s USE_SDL_MIXER=2")
	
	if(HYBUILD_SDL_NET)
		list(APPEND EM_LINK_FLAGS "-s USE_SDL_NET=2")
	endif()
	
	list(APPEND EM_LINK_FLAGS "--no-heap-copy")
	list(APPEND EM_LINK_FLAGS "--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/${REL_DATA_DIR}@./data")
	list(APPEND EM_LINK_FLAGS "--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.hyproj@${PROJECT_NAME}.hyproj")
	
	if(HYBUILD_DebugEmscripten)
		message(STATUS "Building for DEBUG")
		set(CMAKE_BUILD_TYPE Debug)
		target_compile_definitions(HyEngine PRIVATE DEBUG)
		target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
		set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS_FLAGS "-fsanitize=undefined -g")
		
		list(APPEND EM_LINK_FLAGS "-g")
		list(APPEND EM_LINK_FLAGS "-s ASSERTIONS=1")
		list(APPEND EM_LINK_FLAGS "-s SAFE_HEAP=1")
		list(APPEND EM_LINK_FLAGS "-s STACK_OVERFLOW_CHECK=2")
		list(APPEND EM_LINK_FLAGS "-s LLD_REPORT_UNDEFINED")
	endif()
	
	string(REPLACE ";" " " EM_LINK_FLAGS "${EM_LINK_FLAGS}")
	message(STATUS "EM_LINK_FLAGS:" ${EM_LINK_FLAGS})
	
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS ${EM_LINK_FLAGS})
	set(CMAKE_EXECUTABLE_SUFFIX ".html")
else()
	target_precompile_headers(${PROJECT_NAME} PRIVATE "${REL_SRC_DIR}/pch.h")
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC HyEngine %HY_DEPENDENCIES_LINK%)

if(TARGET uninstall)
	set_target_properties(uninstall PROPERTIES FOLDER "CMakePredefinedTargets")
endif()

if (MSVC)
	set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
	set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif()

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/${REL_SRC_DIR}" PREFIX "Source Files" FILES ${GAME_SRCFILES})
