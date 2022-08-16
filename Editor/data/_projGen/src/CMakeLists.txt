cmake_minimum_required(VERSION 3.10)
project(%HY_PROJECTNAME%)

set(REL_PROJ_PATH "%HY_RELPROJPATH%")
set(REL_DATA_DIR "%HY_RELDATADIR%")
set(REL_HARMONY_DIR "%HY_RELHARMONYDIR%")
set(EXE_OUTPUT_NAME "%HY_OUTPUTNAME%")

set(GAME_SRCFILES
%HY_SRCFILES%)

set(GAME_INCLUDE_DIRS "")
list(APPEND GAME_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")
%HY_INCLUDEDIRS%
#################################################################################################
# ^ ^ ^ USER CONFIG ABOVE ^ ^ ^

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Build Options
%HY_USEGLFW%
%HY_USESDLNET%
option(HYBUILD_DebugEmscripten "Use a Debug build configuration when targeting web browsers" OFF)
option(HYBUILD_STATIC "Statically link all dependencies into the executable" OFF)

# Built Extras - Requires installed libraries
%HY_USESPINE%
%HY_USEICU%

if(HYBUILD_GLFW)
	add_compile_definitions(HY_USE_GLFW)
endif()
if(HYBUILD_SPINE)
	add_compile_definitions(HY_USE_SPINE)
endif()
if(HYBUILD_RAPIDJSON)
	add_compile_definitions(HY_USE_RAPIDJSON)
endif()
if(HYBUILD_ICU)
	add_compile_definitions(HY_USE_ICU)
endif()

message(STATUS "%HY_TITLE% building for target: ${CMAKE_SYSTEM_NAME}")
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${REL_PROJ_PATH}/bin/web")
	set(HYEM_COMPILE_FLAGS "")

	list(APPEND HYEM_COMPILE_FLAGS "-s USE_SDL=2")
	list(APPEND HYEM_COMPILE_FLAGS "-s USE_OGG=1")
	list(APPEND HYEM_COMPILE_FLAGS "-s USE_VORBIS=1")
	list(APPEND HYEM_COMPILE_FLAGS "-s USE_SDL_MIXER=2")
	
	set(HYEM_LINK_FLAGS "")
	list(APPEND HYEM_LINK_FLAGS "-s ALLOW_MEMORY_GROWTH=1")
	list(APPEND HYEM_LINK_FLAGS "-s USE_WEBGL2=1")
	list(APPEND HYEM_LINK_FLAGS "-s USE_SDL=2")
	list(APPEND HYEM_LINK_FLAGS "-s USE_OGG=1")
	list(APPEND HYEM_LINK_FLAGS "-s USE_VORBIS=1")
	list(APPEND HYEM_LINK_FLAGS "-s USE_SDL_MIXER=2")
	list(APPEND HYEM_LINK_FLAGS "-s FETCH")
	#list(APPEND HYEM_LINK_FLAGS "-s EXPORTED_RUNTIME_METHODS=ccall")
	#list(APPEND HYEM_LINK_FLAGS "-s EXPORTED_FUNCTIONS=_main,_doPariplayEvent")
	
	if(HYBUILD_ICU)
		list(APPEND HYEM_COMPILE_FLAGS "-s USE_ICU=1")
		list(APPEND HYEM_LINK_FLAGS "-s USE_ICU=1")
	endif()
	
	if(HYBUILD_SDL_NET)
		list(APPEND HYEM_COMPILE_FLAGS "-s USE_SDL_NET=2")
		list(APPEND HYEM_LINK_FLAGS "-s USE_SDL_NET=2")
	endif()
	
	list(APPEND HYEM_LINK_FLAGS "--no-heap-copy")
	list(APPEND HYEM_LINK_FLAGS "--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/${REL_DATA_DIR}@./data")
	list(APPEND HYEM_LINK_FLAGS "--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/${REL_PROJ_PATH}/${PROJECT_NAME}.hyproj@${PROJECT_NAME}.hyproj")
	
	if(HYBUILD_DebugEmscripten)
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${REL_PROJ_PATH}/bin/web/debug")
		message(STATUS "!!! EMSCRIPTEN DEBUG FLAGS ENABLED !!!")
		add_compile_definitions(DEBUG)
		
		list(APPEND HYEM_COMPILE_FLAGS "-g")
		#list(APPEND HYEM_COMPILE_FLAGS "-s ASSERTIONS=1")
		#list(APPEND HYEM_COMPILE_FLAGS "-s STACK_OVERFLOW_CHECK=2")
		#list(APPEND HYEM_COMPILE_FLAGS "-s LLD_REPORT_UNDEFINED")
		
		list(APPEND HYEM_LINK_FLAGS "-g")
		#list(APPEND HYEM_LINK_FLAGS "-s ASSERTIONS=1")
		#list(APPEND HYEM_LINK_FLAGS "-s STACK_OVERFLOW_CHECK=2")
		#list(APPEND HYEM_LINK_FLAGS "-s LLD_REPORT_UNDEFINED")
		
		#list(APPEND HYEM_COMPILE_FLAGS "-s SAFE_HEAP=1")
		#list(APPEND HYEM_LINK_FLAGS "-s SAFE_HEAP=1")
		
		#list(APPEND HYEM_COMPILE_FLAGS "-fsanitize=undefined")
		#list(APPEND HYEM_COMPILE_FLAGS "-fsanitize=address")
		#list(APPEND HYEM_COMPILE_FLAGS "-s TOTAL_MEMORY=2147418112")
		#list(APPEND HYEM_COMPILE_FLAGS "-gsource-map http://localhost:8000/")
		
		#list(APPEND HYEM_LINK_FLAGS "-fsanitize=undefined")
		#list(APPEND HYEM_LINK_FLAGS "-fsanitize=address")
		#list(APPEND HYEM_LINK_FLAGS "-s TOTAL_MEMORY=2147418112")
		#list(APPEND HYEM_LINK_FLAGS "-gsource-map http://localhost:8000/")
	else()
		list(APPEND HYEM_COMPILE_FLAGS "-O2")
	endif()
	
	string(REPLACE ";" " " HYEM_COMPILE_FLAGS "${HYEM_COMPILE_FLAGS}")
	add_compile_options("SHELL:${HYEM_COMPILE_FLAGS}")
	
	string(REPLACE ";" " " HYEM_LINK_FLAGS "${HYEM_LINK_FLAGS}")
	add_link_options("SHELL:${HYEM_LINK_FLAGS}")
	
	set(CMAKE_EXECUTABLE_SUFFIX ".html")
else()
	if(MSVC)
		if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
			set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${REL_PROJ_PATH}/bin/Win32")
		else()
			set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${REL_PROJ_PATH}/bin/x64")
		endif()
	else()
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${REL_PROJ_PATH}/bin/")
	endif()
endif()

add_subdirectory("${REL_HARMONY_DIR}/Engine" "HyEngine")
%HY_DEPENDENCIES_ADD%
add_executable(${PROJECT_NAME} WIN32 ${GAME_SRCFILES})
set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO
	OUTPUT_NAME ${EXE_OUTPUT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE "$<BUILD_INTERFACE:${GAME_INCLUDE_DIRS}>")
if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
	target_precompile_headers(${PROJECT_NAME} PRIVATE "pch.h")
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC HyEngine %HY_DEPENDENCIES_LINK%)


if(TARGET uninstall)
	set_target_properties(uninstall PROPERTIES FOLDER "CMakePredefinedTargets")
endif()

if (MSVC)
	set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/${REL_PROJ_PATH}")
	set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
	
	# Copy OGG/VORBIS DLL's to output directory
	set(HY_AUDIO_RUNTIME_DIR "${REL_HARMONY_DIR}/Engine/vendor/SDL2_mixer/VisualC/external/lib")
	if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
		set(HY_AUDIO_RUNTIME_DIR "${HY_AUDIO_RUNTIME_DIR}/x86")
	else()
		set(HY_AUDIO_RUNTIME_DIR "${HY_AUDIO_RUNTIME_DIR}/x64")
	endif()
	set(HY_AUDIO_RUNTIME_LIBS "")
	file(GLOB HY_AUDIO_RUNTIME_LIBS "${HY_AUDIO_RUNTIME_DIR}/*ogg*")
	file(GLOB HY_TEMP_VORBIS_FILES "${HY_AUDIO_RUNTIME_DIR}/*vorbis*")
	list(APPEND HY_AUDIO_RUNTIME_LIBS ${HY_TEMP_VORBIS_FILES})
	foreach(CUR_RUNTIME_FILE IN LISTS HY_AUDIO_RUNTIME_LIBS)
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CUR_RUNTIME_FILE} $(OutDir)
			COMMENT "Copying audio runtime lib: ${CUR_RUNTIME_FILE}\n")
	endforeach()
	
	if(HYBUILD_ICU)
		set(ICU_RUNTIME_LIBS "")
		file(GLOB_RECURSE ICU_RUNTIME_LIBS "${REL_HARMONY_DIR}/Engine/extras/ICU/*.dll")
		foreach(CUR_RUNTIME_FILE IN LISTS ICU_RUNTIME_LIBS)
			add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CUR_RUNTIME_FILE} $(OutDir)
				COMMENT "Copying ICU runtime lib: ${CUR_RUNTIME_FILE}\n")
		endforeach()
	endif()
endif()

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX "Source Files" FILES ${GAME_SRCFILES})
