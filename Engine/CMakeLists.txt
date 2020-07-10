add_subdirectory(external/box2d)
add_subdirectory(external/soil2)

# Emscripten provides its own ports of SDL and GLEW
if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
	add_subdirectory(external/glad)
	
	# Qt provides its own implementation of SDL facitlities
	if(NOT HYBUILD_Qt)
		# Below is typical desktop deployment
		add_subdirectory(external/SDL2)
		include_directories(external/SDL2/include)
		set_target_properties(SDL2main PROPERTIES FOLDER "Engine Libs")
		if(SDL_SHARED)
			set_target_properties(SDL2 PROPERTIES FOLDER "Engine Libs")
		endif()
		if(SDL_STATIC)
			set_target_properties(SDL2-static PROPERTIES FOLDER "Engine Libs")
		endif()
		
		add_subdirectory(external/SDL2_mixer)
		
		# Optional SDL2 extensions
		set(HYBUILD_SDL_NET ON)
		if(HYBUILD_SDL_NET)
			add_subdirectory(external/SDL2_net)
		endif()
	endif()
endif()

set(HYENGINE_SOURCE_FILES
	src/HyEngine.cpp
	src/Afx/HyStdAfx.cpp
	src/Assets/HyAssets.cpp
	src/Assets/Files/HyFileAtlas.cpp
	src/Assets/Files/HyFileAudio.cpp
	src/Assets/Files/HyFilesManifest.cpp
	src/Assets/Files/HyGLTF.cpp
	src/Assets/Nodes/HyAudioData.cpp
	src/Assets/Nodes/HyEntityData.cpp
	src/Assets/Nodes/HyPrefabData.cpp
	src/Assets/Nodes/HySpine2dData.cpp
	src/Assets/Nodes/HySprite2dData.cpp
	src/Assets/Nodes/HyText2dData.cpp
	src/Assets/Nodes/HyTexturedQuad2dData.cpp
	src/Assets/Nodes/IHyNodeData.cpp
	src/Audio/HyAudioHarness.cpp
	src/Audio/Harness/SDL2/HyAudioCore_SDL2.cpp
	src/Audio/Harness/SDL2/HyFileAudioImpl_SDL2.cpp
	src/Diagnostics/HyDiagnostics.cpp
	src/Diagnostics/Console/HyConsole.cpp
	src/Diagnostics/Console/Interop/HyConsole_Std.cpp
	src/Diagnostics/Console/Interop/HyConsole_Win.cpp
	src/Diagnostics/GuiComms/HyGuiComms.cpp
	src/Diagnostics/GuiComms/HyGuiMessage.cpp
	src/Diagnostics/GuiComms/HyLiveVarManager.cpp
	src/Diagnostics/Output/HyDiagOutput.cpp
	src/Diagnostics/Output/HyProfiler.cpp
	src/Input/HyInput.cpp
	src/Input/HyInputMap.cpp
	src/Networking/HyNetworking.cpp
	src/Renderer/IHyRenderer.cpp
	src/Renderer/Components/HyRenderBuffer.cpp
	src/Renderer/Components/HyShaderUniforms.cpp
	src/Renderer/Components/HyVertexBuffer.cpp
	src/Renderer/Effects/HyShader.cpp
	src/Renderer/Effects/HyStencil.cpp
	src/Renderer/OpenGL/HyOpenGL.cpp
	src/Scene/HyScene.cpp
	src/Scene/AnimFloats/HyAnimFloat.cpp
	src/Scene/AnimFloats/HyAnimVec2.cpp
	src/Scene/AnimFloats/HyAnimVec3.cpp
	src/Scene/AnimFloats/HyTweenFuncs.cpp
	src/Scene/Nodes/IHyNode.cpp
	src/Scene/Nodes/IHyNode2d.cpp
	src/Scene/Nodes/IHyNode3d.cpp
	src/Scene/Nodes/Loadables/IHyLoadable.cpp
	src/Scene/Nodes/Loadables/IHyLoadable2d.cpp
	src/Scene/Nodes/Loadables/IHyLoadable3d.cpp
	src/Scene/Nodes/Loadables/Drawables/IHyDrawable.cpp
	src/Scene/Nodes/Loadables/Drawables/IHyDrawable2d.cpp
	src/Scene/Nodes/Loadables/Drawables/IHyDrawable3d.cpp
	src/Scene/Nodes/Loadables/Drawables/Instances/IHyInstance.cpp
	src/Scene/Nodes/Loadables/Drawables/Instances/IHyInstance2d.cpp
	src/Scene/Nodes/Loadables/Drawables/Instances/IHyInstance3d.cpp
	src/Scene/Nodes/Loadables/Drawables/Instances/Objects/HyPrefab3d.cpp
	src/Scene/Nodes/Loadables/Drawables/Instances/Objects/HyPrimitive2d.cpp
	src/Scene/Nodes/Loadables/Drawables/Instances/Objects/HySpine2d.cpp
	src/Scene/Nodes/Loadables/Drawables/Instances/Objects/HySprite2d.cpp
	src/Scene/Nodes/Loadables/Drawables/Instances/Objects/HyText2d.cpp
	src/Scene/Nodes/Loadables/Drawables/Instances/Objects/HyTexturedQuad2d.cpp
	src/Scene/Nodes/Loadables/Drawables/Objects/HyEntity2d.cpp
	src/Scene/Nodes/Loadables/Drawables/Objects/HyEntity3d.cpp
	src/Scene/Nodes/Loadables/Objects/HyAudio2d.cpp
	src/Scene/Nodes/Loadables/Objects/HyAudio3d.cpp
	src/Scene/Nodes/Objects/HyCamera.cpp
	src/Scene/Physics/HyBox2dRuntime.cpp
	src/Scene/Physics/HyPhysEntity2d.cpp
	src/Scene/Physics/HyPhysicsCollider.cpp
	src/Scene/Physics/HyPhysicsDebug2d.cpp
	src/Scene/Physics/HyPhysicsGrid2d.cpp
	src/Scene/Physics/HyShape2d.cpp
	src/Threading/IHyThreadClass.cpp
	src/Time/HyTime.cpp
	src/Time/Watches/HyStopwatch.cpp
	src/Time/Watches/HyTimer.cpp
	src/Time/Watches/IHyTimeInst.cpp
	src/UI/HyButton.cpp
	src/UI/HyInfoPanel.cpp
	src/UI/HyMeter.cpp
	src/UI/IHy9Slice.cpp
	src/Utilities/jasonxx.cpp
	src/Utilities/Crc32.cpp
	src/Utilities/HyIO.cpp
	src/Utilities/HyMath.cpp
	src/Window/HyWindow.cpp
	src/Window/HyWindowManager.cpp)

set(HYENGINE_HEADER_FILES
	include/Harmony/HyEngine.h
	include/Harmony/Afx/HyInteropAfx.h
	include/Harmony/Afx/HyStdAfx.h
	include/Harmony/Afx/Compilers/HyCompiler.h
	include/Harmony/Afx/Compilers/HyCompiler_GNU.h
	include/Harmony/Afx/Compilers/HyCompiler_MSVC.h
	include/Harmony/Afx/Compilers/HyCompiler_MWerks.h
	include/Harmony/Afx/Platforms/HyPlatAfx.h
	include/Harmony/Afx/Platforms/HyPlatform.h
	include/Harmony/Afx/Platforms/HyPlatform_Browser.h
	include/Harmony/Afx/Platforms/HyPlatform_Gui.h
	include/Harmony/Afx/Platforms/HyPlatform_OSX.h
	include/Harmony/Afx/Platforms/HyPlatform_Unix.h
	include/Harmony/Afx/Platforms/HyPlatform_Windows.h
	include/Harmony/Assets/HyAssets.h
	include/Harmony/Assets/Files/HyFileAtlas.h
	include/Harmony/Assets/Files/HyFileAudio.h
	include/Harmony/Assets/Files/HyFilesManifest.h
	include/Harmony/Assets/Files/HyGLTF.h
	include/Harmony/Assets/Files/IHyFile.h
	include/Harmony/Assets/Nodes/HyAudioData.h
	include/Harmony/Assets/Nodes/HyEntityData.h
	include/Harmony/Assets/Nodes/HyPrefabData.h
	include/Harmony/Assets/Nodes/HySpine2dData.h
	include/Harmony/Assets/Nodes/HySprite2dData.h
	include/Harmony/Assets/Nodes/HyText2dData.h
	include/Harmony/Assets/Nodes/HyTexturedQuad2dData.h
	include/Harmony/Assets/Nodes/IHyNodeData.h
	include/Harmony/Audio/HyAudioHarness.h
	include/Harmony/Audio/Harness/IHyAudioCore.h
	include/Harmony/Audio/Harness/IHyFileAudioImpl.h
	include/Harmony/Audio/Harness/SDL2/HyAudioCore_SDL2.h
	include/Harmony/Audio/Harness/SDL2/HyFileAudioImpl_SDL2.h
	include/Harmony/Diagnostics/HyDiagnostics.h
	include/Harmony/Diagnostics/Console/HyConsole.h
	include/Harmony/Diagnostics/Console/Interop/HyConsole_Std.h
	include/Harmony/Diagnostics/Console/Interop/HyConsole_Win.h
	include/Harmony/Diagnostics/GuiComms/HyGuiComms.h
	include/Harmony/Diagnostics/GuiComms/HyGuiMessage.h
	include/Harmony/Diagnostics/GuiComms/HyLiveVarManager.h
	include/Harmony/Diagnostics/Output/HyDiagOutput.h
	include/Harmony/Diagnostics/Output/HyProfiler.h
	include/Harmony/Input/HyInput.h
	include/Harmony/Input/HyInputKeys.h
	include/Harmony/Input/HyInputMap.h
	include/Harmony/Networking/HyNetworking.h
	include/Harmony/Renderer/IHyRenderer.h
	include/Harmony/Renderer/Components/HyRenderBuffer.h
	include/Harmony/Renderer/Components/HyShaderUniforms.h
	include/Harmony/Renderer/Components/HyVertexBuffer.h
	include/Harmony/Renderer/Effects/HyShader.h
	include/Harmony/Renderer/Effects/HyStencil.h
	include/Harmony/Renderer/OpenGL/HyOpenGL.h
	include/Harmony/Renderer/OpenGL/HyOpenGLESShaderSrc.h
	include/Harmony/Renderer/OpenGL/HyOpenGLShaderSrc.h
	include/Harmony/Scene/HyScene.h
	include/Harmony/Scene/AnimFloats/HyAnimFloat.h
	include/Harmony/Scene/AnimFloats/HyAnimVec2.h
	include/Harmony/Scene/AnimFloats/HyAnimVec3.h
	include/Harmony/Scene/AnimFloats/HyTweenFuncs.h
	include/Harmony/Scene/Nodes/IHyNode.h
	include/Harmony/Scene/Nodes/IHyNode2d.h
	include/Harmony/Scene/Nodes/IHyNode3d.h
	include/Harmony/Scene/Nodes/Loadables/IHyLoadable.h
	include/Harmony/Scene/Nodes/Loadables/IHyLoadable2d.h
	include/Harmony/Scene/Nodes/Loadables/IHyLoadable3d.h
	include/Harmony/Scene/Nodes/Loadables/Drawables/IHyDrawable.h
	include/Harmony/Scene/Nodes/Loadables/Drawables/IHyDrawable2d.h
	include/Harmony/Scene/Nodes/Loadables/Drawables/IHyDrawable3d.h
	include/Harmony/Scene/Nodes/Loadables/Drawables/Instances/IHyInstance.h
	include/Harmony/Scene/Nodes/Loadables/Drawables/Instances/IHyInstance2d.h
	include/Harmony/Scene/Nodes/Loadables/Drawables/Instances/IHyInstance3d.h
	include/Harmony/Scene/Nodes/Loadables/Drawables/Instances/Objects/HyPrefab3d.h
	include/Harmony/Scene/Nodes/Loadables/Drawables/Instances/Objects/HyPrimitive2d.h
	include/Harmony/Scene/Nodes/Loadables/Drawables/Instances/Objects/HySpine2d.h
	include/Harmony/Scene/Nodes/Loadables/Drawables/Instances/Objects/HySprite2d.h
	include/Harmony/Scene/Nodes/Loadables/Drawables/Instances/Objects/HyText2d.h
	include/Harmony/Scene/Nodes/Loadables/Drawables/Instances/Objects/HyTexturedQuad2d.h
	include/Harmony/Scene/Nodes/Loadables/Drawables/Objects/HyEntity2d.h
	include/Harmony/Scene/Nodes/Loadables/Drawables/Objects/HyEntity3d.h
	include/Harmony/Scene/Nodes/Loadables/Drawables/Objects/HyEntityLeaf2d.h
	include/Harmony/Scene/Nodes/Loadables/Objects/IHyAudio.h
	include/Harmony/Scene/Nodes/Loadables/Objects/HyAudio2d.h
	include/Harmony/Scene/Nodes/Loadables/Objects/HyAudio3d.h
	include/Harmony/Scene/Nodes/Objects/HyCamera.h
	include/Harmony/Scene/Nodes/Objects/IHyCamera.h
	include/Harmony/Scene/Physics/HyBox2dRuntime.h
	include/Harmony/Scene/Physics/HyPhysEntity2d.h
	include/Harmony/Scene/Physics/HyPhysicsCollider.h
	include/Harmony/Scene/Physics/HyPhysicsDebug2d.h
	include/Harmony/Scene/Physics/HyPhysicsGrid2d.h
	include/Harmony/Scene/Physics/HyShape2d.h
	include/Harmony/Threading/IHyThreadClass.h
	include/Harmony/Time/HyTime.h
	include/Harmony/Time/Watches/HyStopwatch.h
	include/Harmony/Time/Watches/HyTimer.h
	include/Harmony/Time/Watches/IHyTimeInst.h
	include/Harmony/UI/HyButton.h
	include/Harmony/UI/HyInfoPanel.h
	include/Harmony/UI/HyMeter.h
	include/Harmony/UI/IHy9Slice.h
	include/Harmony/Utilities/jsonxx.h
	include/Harmony/Utilities/Crc32.h
	include/Harmony/Utilities/HyIO.h
	include/Harmony/Utilities/HyMath.h
	include/Harmony/Utilities/HyRand.h
	include/Harmony/Window/HyWindow.h
	include/Harmony/Window/HyWindowManager.h)

add_library(HyEngine STATIC ${HYENGINE_SOURCE_FILES} ${HYENGINE_HEADER_FILES} include/tinygltf/tiny_gltf.h)
set_target_properties(HyEngine PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO)
	
target_include_directories(HyEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(HyEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/Harmony")
target_include_directories(HyEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/box2d/include")
target_include_directories(HyEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/soil2/include")

target_link_libraries(HyEngine PUBLIC box2d soil2)

# Emscripten provides its own ports of SDL and GLEW
if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
	target_include_directories(HyEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include")
	target_link_libraries(HyEngine PUBLIC glad)
	target_precompile_headers(HyEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/Harmony/Afx/HyStdAfx.h")
	
	# Qt provides its own implementation of SDL facitlities
	if(NOT HYBUILD_Qt)
		# Below is typical desktop deployment
		target_include_directories(HyEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL2_mixer")
		target_link_libraries(HyEngine PUBLIC SDL2 SDL2main SDL2_mixer)
		
		if(HYBUILD_SDL_NET)
			target_include_directories(HyEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL2_net")
			target_link_libraries(HyEngine PUBLIC SDL2_net)
			target_compile_definitions(HyEngine PRIVATE HY_USE_SDL2_NET)
		endif()
	endif()
endif()

if (MSVC)
	target_compile_definitions(HyEngine PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" FILES ${HYENGINE_SOURCE_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include/Harmony/" FILES ${HYENGINE_HEADER_FILES})
