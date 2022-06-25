# BUILD OPTIONS: HYBUILD_Qt, HYBUILD_SDL_NET, HYBUILD_GLFW, HYBUILD_STATIC
# BUILD EXTRAS:  HYBUILD_SPINE, HYBUILD_ICU

add_subdirectory(external/box2d)
add_subdirectory(external/soil2)

if(HYBUILD_SPINE)
	add_subdirectory(extras/Spine/spine-cpp)
	set_target_properties(spine-cpp PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO
	FOLDER "Engine Libs")
endif()

# Emscripten provides its own ports of SDL, GLFW, and GLEW
if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
	add_subdirectory(external/glad)
	
	# Qt provides its own implementation of SDL/GLFW facilities
	if(HYBUILD_Qt)
		set(HYBUILD_SDL_NET OFF)
		set(HYBUILD_GLFW OFF)
	else()
		# Below is typical desktop deployment
		if(HYBUILD_STATIC)
			set(SDL_STATIC ON)
			set(SDL_SHARED OFF)
		else()
			set(SDL_SHARED ON)
		endif()
		
		add_subdirectory(external/SDL2)
		include_directories(external/SDL2/include)
		set_target_properties(SDL2main PROPERTIES FOLDER "Engine Libs")
		set_target_properties(SDL2-static PROPERTIES FOLDER "Engine Libs")
		set_target_properties(SDL2 PROPERTIES FOLDER "Engine Libs")
		
		add_subdirectory(external/SDL2_mixer)

		# Optional SDL2 extensions
		if(HYBUILD_SDL_NET)
			add_subdirectory(external/SDL2_net)
		endif()
		
		if(HYBUILD_GLFW)
			add_compile_definitions(HY_USE_GLFW)
			set(GLFW_BUILD_EXAMPLES, OFF)
			set(GLFW_BUILD_TESTS, OFF)
			set(GLFW_BUILD_DOCS, OFF)
			set(GLFW_INSTALL, OFF)
			add_subdirectory(external/glfw)
			set_target_properties(glfw PROPERTIES FOLDER "Engine Libs")
		endif()
	endif()
else()
	set(HYBUILD_STATIC ON)
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
	src/Assets/Nodes/HyPrefabData.cpp
	src/Assets/Nodes/HySpineData.cpp
	src/Assets/Nodes/HySpriteData.cpp
	src/Assets/Nodes/HyTextData.cpp
	src/Assets/Nodes/HyTexturedQuadData.cpp
	src/Assets/Nodes/IHyNodeData.cpp
	src/Audio/IHyAudioCore.cpp
	src/Audio/SDL2/HyAudioCore_SDL2.cpp
	src/Audio/SDL2/HyFileAudioImpl_SDL2.cpp
	src/Diagnostics/HyDiagnostics.cpp
	src/Diagnostics/Console/IHyConsole.cpp
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
	src/Networking/IHyNetworkClass.cpp
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
	src/Scene/Nodes/Loadables/Bodies/IHyBody.cpp
	src/Scene/Nodes/Loadables/Bodies/IHyBody2d.cpp
	src/Scene/Nodes/Loadables/Bodies/IHyBody3d.cpp
	src/Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable.cpp
	src/Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable2d.cpp
	src/Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable3d.cpp
	src/Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyPrefab3d.cpp
	src/Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyPrimitive2d.cpp
	src/Scene/Nodes/Loadables/Bodies/Drawables/Objects/HySpine2d.cpp
	src/Scene/Nodes/Loadables/Bodies/Drawables/Objects/HySprite2d.cpp
	src/Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyText2d.cpp
	src/Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyTexturedQuad2d.cpp
	src/Scene/Nodes/Loadables/Bodies/Drawables/Objects/IHySprite.cpp
	src/Scene/Nodes/Loadables/Bodies/Drawables/Objects/IHySpine.cpp
	src/Scene/Nodes/Loadables/Bodies/Drawables/Objects/IHyText.cpp
	src/Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.cpp
	src/Scene/Nodes/Loadables/Bodies/Objects/HyEntity3d.cpp
	src/Scene/Nodes/Loadables/Bodies/Objects/HyPhysicsGrid2d.cpp
	src/Scene/Nodes/Loadables/Objects/HyAudio2d.cpp
	src/Scene/Nodes/Loadables/Objects/HyAudio3d.cpp
	src/Scene/Nodes/Loadables/Objects/IHyAudio.cpp
	src/Scene/Nodes/Objects/HyCamera.cpp
	src/Scene/Nodes/Objects/IHyCamera.cpp
	src/Scene/Physics/HyBox2dRuntime.cpp
	src/Scene/Physics/HyPhysicsCtrl2d.cpp
	src/Scene/Physics/HyPhysicsDebug2d.cpp
	src/Scene/Physics/HyShape2d.cpp
	src/Threading/IHyThreadClass.cpp
	src/Time/HyTime.cpp
	src/Time/Watches/HyStopwatch.cpp
	src/Time/Watches/HyTimer.cpp
	src/Time/Watches/IHyTimeInst.cpp
	src/UI/IHyEntityUi.cpp
	src/UI/HyUiContainer.cpp
	src/UI/Widgets/HyButton.cpp
	src/UI/Widgets/HyCheckBox.cpp
	src/UI/Widgets/HyComboBox.cpp
	src/UI/Widgets/HyLabel.cpp
	src/UI/Widgets/HyLineEdit.cpp
	src/UI/Widgets/HyProgressBar.cpp
	src/UI/Widgets/HyRackMeter.cpp
	src/UI/Widgets/HyRichText.cpp
	src/UI/Widgets/HySlider.cpp
	src/UI/Widgets/IHyWidget.cpp
	src/UI/Components/HyCloseButton.cpp
	src/UI/Components/HyLayout.cpp
	src/UI/Components/HyPanel.cpp
	src/UI/Components/HyScrollBar.cpp
	src/UI/Components/HySpacer.cpp
	src/UI/Localization/HyLocale.cpp
	src/UI/Localization/HyNumberFormat.cpp
	src/UI/Localization/HyTranslateTable.cpp
	src/Utilities/Crc32.cpp
	src/Utilities/HyIO.cpp
	src/Utilities/HyMath.cpp
	src/Window/HyWindow.cpp
	src/Window/HyWindowManager.cpp)

set(HYENGINE_HEADER_FILES
	include/Harmony/HyEngine.h
	include/Harmony/Afx/HyFlags.h
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
	include/Harmony/Assets/Nodes/HyPrefabData.h
	include/Harmony/Assets/Nodes/HySpineData.h
	include/Harmony/Assets/Nodes/HySpriteData.h
	include/Harmony/Assets/Nodes/HyTextData.h
	include/Harmony/Assets/Nodes/HyTexturedQuadData.h
	include/Harmony/Assets/Nodes/IHyNodeData.h
	include/Harmony/Audio/IHyAudioCore.h
	include/Harmony/Audio/IHyFileAudioImpl.h
	include/Harmony/Audio/SDL2/HyAudioCore_SDL2.h
	include/Harmony/Audio/SDL2/HyFileAudioImpl_SDL2.h
	include/Harmony/Diagnostics/HyDiagnostics.h
	include/Harmony/Diagnostics/Console/IHyConsole.h
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
	include/Harmony/Networking/IHyNetworkClass.h
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
	include/Harmony/Scene/Nodes/Loadables/Bodies/IHyBody.h
	include/Harmony/Scene/Nodes/Loadables/Bodies/IHyBody2d.h
	include/Harmony/Scene/Nodes/Loadables/Bodies/IHyBody3d.h
	include/Harmony/Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable.h
	include/Harmony/Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable2d.h
	include/Harmony/Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable3d.h
	include/Harmony/Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyPrefab3d.h
	include/Harmony/Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyPrimitive2d.h
	include/Harmony/Scene/Nodes/Loadables/Bodies/Drawables/Objects/HySpine2d.h
	include/Harmony/Scene/Nodes/Loadables/Bodies/Drawables/Objects/HySprite2d.h
	include/Harmony/Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyText2d.h
	include/Harmony/Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyTexturedQuad2d.h
	include/Harmony/Scene/Nodes/Loadables/Bodies/Drawables/Objects/IHySprite.h
	include/Harmony/Scene/Nodes/Loadables/Bodies/Drawables/Objects/IHySpine.h
	include/Harmony/Scene/Nodes/Loadables/Bodies/Drawables/Objects/IHyText.h
	include/Harmony/Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h
	include/Harmony/Scene/Nodes/Loadables/Bodies/Objects/HyEntity3d.h
	include/Harmony/Scene/Nodes/Loadables/Bodies/Objects/HyEntityLeaf2d.h
	include/Harmony/Scene/Nodes/Loadables/Bodies/Objects/HyPhysicsGrid2d.h
	include/Harmony/Scene/Nodes/Loadables/Objects/IHyAudio.h
	include/Harmony/Scene/Nodes/Loadables/Objects/HyAudio2d.h
	include/Harmony/Scene/Nodes/Loadables/Objects/HyAudio3d.h
	include/Harmony/Scene/Nodes/Objects/HyCamera.h
	include/Harmony/Scene/Nodes/Objects/IHyCamera.h
	include/Harmony/Scene/Physics/HyBox2dRuntime.h
	include/Harmony/Scene/Physics/HyPhysicsCtrl2d.h
	include/Harmony/Scene/Physics/HyPhysicsDebug2d.h
	include/Harmony/Scene/Physics/HyShape2d.h
	include/Harmony/Threading/IHyThreadClass.h
	include/Harmony/Time/HyTime.h
	include/Harmony/Time/Watches/HyStopwatch.h
	include/Harmony/Time/Watches/HyTimer.h
	include/Harmony/Time/Watches/IHyTimeInst.h
	include/Harmony/UI/IHyEntityUi.h
	include/Harmony/UI/HyUiContainer.h
	include/Harmony/UI/Widgets/HyButton.h
	include/Harmony/UI/Widgets/HyCheckBox.h
	include/Harmony/UI/Widgets/HyComboBox.h
	include/Harmony/UI/Widgets/HyLabel.h
	include/Harmony/UI/Widgets/HyLineEdit.h
	include/Harmony/UI/Widgets/HyProgressBar.h
	include/Harmony/UI/Widgets/HyRackMeter.h
	include/Harmony/UI/Widgets/HyRichText.h
	include/Harmony/UI/Widgets/HySlider.h
	include/Harmony/UI/Widgets/IHyWidget.h
	include/Harmony/UI/Components/HyCloseButton.h
	include/Harmony/UI/Components/HyLayout.h
	include/Harmony/UI/Components/HyPanel.h
	include/Harmony/UI/Components/HyScrollBar.h
	include/Harmony/UI/Components/HySpacer.h
	include/Harmony/UI/Localization/HyLocale.h
	include/Harmony/UI/Localization/HyNumpunct.h
	include/Harmony/UI/Localization/HyNumberFormat.h
	include/Harmony/UI/Localization/HyTranslateTable.h
	include/Harmony/Utilities/Crc32.h
	include/Harmony/Utilities/HyIO.h
	include/Harmony/Utilities/HyMath.h
	include/Harmony/Utilities/HyRand.h
	include/Harmony/Window/HyWindow.h
	include/Harmony/Window/HyWindowManager.h)

add_library(HyEngine STATIC ${HYENGINE_SOURCE_FILES} ${HYENGINE_HEADER_FILES} include/tinygltf/tiny_gltf.h)
set_target_properties(HyEngine PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO)
	
target_include_directories(HyEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(HyEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/Harmony")
target_include_directories(HyEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/box2d/include")
target_include_directories(HyEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/soil2/include")

target_link_libraries(HyEngine PUBLIC box2d soil2)

if(HYBUILD_SPINE)
	target_link_libraries(HyEngine PUBLIC spine-cpp)
endif()

# Emscripten provides its own ports of SDL and GLEW
if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
	target_include_directories(HyEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include")
	target_link_libraries(HyEngine PUBLIC glad)
	target_precompile_headers(HyEngine PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/Harmony/Afx/HyStdAfx.h")
	
	# Qt provides its own implementation of SDL facitlities
	if(NOT HYBUILD_Qt)
		# Below is typical desktop deployment
		target_include_directories(HyEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL2_mixer")
		target_link_libraries(HyEngine PUBLIC SDL2 SDL2main SDL2_mixer)
		
		if(HYBUILD_SDL_NET)
			target_include_directories(HyEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL2_net")
			target_link_libraries(HyEngine PUBLIC SDL2_net)
		endif()
		
		if(HYBUILD_GLFW)
			target_include_directories(HyEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/glfw")
			target_link_libraries(HyEngine PUBLIC glfw)
		endif()
		
		if(HYBUILD_ICU)
			# ICU - International Components for Unicode (aka Insane Clown Union)
			# ICU Components Reference: https://cmake.org/cmake/help/latest/module/FindICU.html
			
			cmake_policy(SET CMP0074 NEW) # find_package() command now searches prefixes specified by <PackageName>_ROOT
			#set(ICU_DEBUG ON)
			set(ICU_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/extras/ICU")
			if(WIN32)
				find_package(ICU 69.0 COMPONENTS uc in REQUIRED)
				target_link_libraries(HyEngine PUBLIC ICU::uc ICU::in)
			else()
				find_package(ICU 69.0 COMPONENTS uc i18n REQUIRED)
				target_link_libraries(HyEngine PUBLIC ICU::uc ICU::i18n)
			endif()
		endif()
	endif()
endif()

if(HYBUILD_SDL_NET)
	target_compile_definitions(HyEngine PUBLIC HY_USE_SDL2_NET)
endif()

if (MSVC)
	target_compile_definitions(HyEngine PUBLIC _CRT_SECURE_NO_WARNINGS)
endif()

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${HYENGINE_SOURCE_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include/Harmony/" PREFIX "Source Files" FILES ${HYENGINE_HEADER_FILES})
