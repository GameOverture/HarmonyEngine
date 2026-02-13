set(Qt6_DIR "" CACHE PATH "Specify cmake Qt6 directory. Ex. C:/Qt/<version>/msvc2022_64/lib/cmake/Qt6")

if(Qt6_DIR)
	message(STATUS "Adding Vendor: freetype2")
	add_subdirectory(vendor/freetype2)
	set_target_properties(freetype PROPERTIES FOLDER "Editor Libs")
	
	cmake_policy(SET CMP0079 NEW)
	message(STATUS "Adding Vendor: freetype-gl")
	add_subdirectory(vendor/freetype-gl)
	set_target_properties(freetype-gl PROPERTIES FOLDER "Editor Libs")
	target_include_directories(freetype-gl PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../Engine/vendor/glad/include")
	target_include_directories(freetype-gl PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/vendor/freetype2/include")
	target_link_libraries(freetype-gl PUBLIC freetype)
	
	set(BUILD_TESTING OFF)
	
	message(STATUS "Adding Vendor: ogg")
	add_subdirectory(vendor/ogg)
	set_target_properties(ogg PROPERTIES FOLDER "Editor Libs")
	
	message(STATUS "Adding Vendor: vorbis")
	add_subdirectory(vendor/vorbis)
	set_target_properties(vorbis PROPERTIES FOLDER "Editor Libs")
	set_target_properties(vorbisenc PROPERTIES FOLDER "Editor Libs")
	set_target_properties(vorbisfile PROPERTIES FOLDER "Editor Libs")
	
	message(STATUS "Adding Vendor: QCodeEditor")
	add_subdirectory(vendor/QCodeEditor)
	set_target_properties(QCodeEditor PROPERTIES FOLDER "Editor Libs")

	message(STATUS "Setting up HyEditor...")
	find_package(Qt6 COMPONENTS Core Gui Widgets OpenGLWidgets Multimedia REQUIRED)

	set(HYEDITOR_SOURCE_FILES
		src/main.cpp
		src/MainWindow.cpp
		src/MainWindow.h
		src/MainWindow.ui
		src/Themes.h
		src/VersionPatcher.cpp
		src/VersionPatcher.h
		src/HyEditor.rc
		src/Assets/AssetMimeData.cpp
		src/Assets/AssetMimeData.h
		src/Assets/BanksModel.h
		src/Assets/BanksModel.cpp
		src/Assets/IAssetItemData.cpp
		src/Assets/IAssetItemData.h
		src/Assets/IImportThread.cpp
		src/Assets/IImportThread.h
		src/Assets/IManagerDraw.cpp
		src/Assets/IManagerDraw.h
		src/Assets/IManagerModel.cpp
		src/Assets/IManagerModel.h
		src/Assets/IRepackThread.cpp
		src/Assets/IRepackThread.h
		src/Assets/ManagerWidget.cpp
		src/Assets/ManagerWidget.h
		src/Assets/ManagerWidget.ui
		src/Assets/Source/DlgAddClassFiles.cpp
		src/Assets/Source/DlgAddClassFiles.h
		src/Assets/Source/DlgAddClassFiles.ui
		src/Assets/Source/DlgBuildSettings.cpp
		src/Assets/Source/DlgBuildSettings.h
		src/Assets/Source/DlgBuildSettings.ui
		src/Assets/Source/DlgNewBuild.cpp
		src/Assets/Source/DlgNewBuild.h
		src/Assets/Source/DlgNewBuild.ui
		src/Assets/Source/DlgPackageBuild.cpp
		src/Assets/Source/DlgPackageBuild.h
		src/Assets/Source/DlgPackageBuild.ui
		src/Assets/Source/SourceFile.cpp
		src/Assets/Source/SourceFile.h
		src/Assets/Source/SourceModel.cpp
		src/Assets/Source/SourceModel.h
		src/Assets/Source/SourceImportThread.cpp
		src/Assets/Source/SourceImportThread.h
		src/Assets/Source/WgtSrcDependency.cpp
		src/Assets/Source/WgtSrcDependency.h
		src/Assets/Source/WgtSrcDependency.ui
		src/Assets/Atlas/AtlasDraw.cpp
		src/Assets/Atlas/AtlasDraw.h
		src/Assets/Atlas/AtlasFrame.cpp
		src/Assets/Atlas/AtlasFrame.h
		src/Assets/Atlas/AtlasImportThread.cpp
		src/Assets/Atlas/AtlasImportThread.h
		src/Assets/Atlas/AtlasModel.cpp
		src/Assets/Atlas/AtlasModel.h
		src/Assets/Atlas/AtlasPacker.cpp
		src/Assets/Atlas/AtlasPacker.h
		src/Assets/Atlas/AtlasRepackThread.cpp
		src/Assets/Atlas/AtlasRepackThread.h
		src/Assets/Atlas/AtlasTileSet.cpp
		src/Assets/Atlas/AtlasTileSet.h
		src/Assets/Atlas/DlgSliceSpriteSheet.cpp
		src/Assets/Atlas/DlgSliceSpriteSheet.h
		src/Assets/Atlas/DlgSliceSpriteSheet.ui
		src/Assets/Atlas/TileSet/IWgtTileSetItem.cpp
		src/Assets/Atlas/TileSet/IWgtTileSetItem.h
		src/Assets/Atlas/TileSet/TileData.cpp
		src/Assets/Atlas/TileSet/TileData.h
		src/Assets/Atlas/TileSet/TileSetGfxItem.cpp
		src/Assets/Atlas/TileSet/TileSetGfxItem.h
		src/Assets/Atlas/TileSet/TileSetScene.cpp
		src/Assets/Atlas/TileSet/TileSetScene.h
		src/Assets/Atlas/TileSet/TileSetUndoCmds.cpp
		src/Assets/Atlas/TileSet/TileSetUndoCmds.h
		src/Assets/Atlas/TileSet/TileSetView.cpp
		src/Assets/Atlas/TileSet/TileSetView.h
		src/Assets/Atlas/TileSet/WgtTileSetAnimation.cpp
		src/Assets/Atlas/TileSet/WgtTileSetAnimation.h
		src/Assets/Atlas/TileSet/WgtTileSetAnimation.ui
		src/Assets/Atlas/TileSet/WgtTileSetCollision.cpp
		src/Assets/Atlas/TileSet/WgtTileSetCollision.h
		src/Assets/Atlas/TileSet/WgtTileSetCollision.ui
		src/Assets/Atlas/TileSet/WgtTileSetTerrain.cpp
		src/Assets/Atlas/TileSet/WgtTileSetTerrain.h
		src/Assets/Atlas/TileSet/WgtTileSetTerrain.ui
		src/Assets/Atlas/TileSet/WgtTileSetTerrainSet.cpp
		src/Assets/Atlas/TileSet/WgtTileSetTerrainSet.h
		src/Assets/Atlas/TileSet/WgtTileSetTerrainSet.ui
		src/Assets/Audio/AudioAssetsWidget.cpp
		src/Assets/Audio/AudioAssetsWidget.h
		src/Assets/Audio/AudioAssetsWidget.ui
		src/Assets/Audio/AudioImportThread.cpp
		src/Assets/Audio/AudioImportThread.h
		src/Assets/Audio/AudioManagerModel.cpp
		src/Assets/Audio/AudioManagerModel.h
		src/Assets/Audio/AudioRepackThread.cpp
		src/Assets/Audio/AudioRepackThread.h
		src/Assets/Audio/SoundClip.cpp
		src/Assets/Audio/SoundClip.h
		src/Items/IDraw.cpp
		src/Items/IDraw.h
		src/Items/IDrawEx.cpp
		src/Items/IDrawEx.h
		src/Items/IDrawExItem.cpp
		src/Items/IDrawExItem.h
		src/Items/IModel.cpp
		src/Items/IModel.h
		src/Items/IWidget.cpp
		src/Items/IWidget.h
		src/Items/IWidget.ui
		src/Items/Spine/SpineCrossFadeModel.cpp
		src/Items/Spine/SpineCrossFadeModel.h
		src/Items/Spine/SpineDraw.cpp
		src/Items/Spine/SpineDraw.h
		src/Items/Spine/SpineModel.cpp
		src/Items/Spine/SpineModel.h
		src/Items/Spine/SpineSkinTreeModel.cpp
		src/Items/Spine/SpineSkinTreeModel.h
		src/Items/Spine/SpineUndoCmds.cpp
		src/Items/Spine/SpineUndoCmds.h
		src/Items/Spine/SpineWidget.cpp
		src/Items/Spine/SpineWidget.h
		src/Items/Spine/SpineWidget.ui
		src/Items/Sprite/SpriteDraw.cpp
		src/Items/Sprite/SpriteModels.cpp
		src/Items/Sprite/SpriteTableView.cpp
		src/Items/Sprite/SpriteUndoCmds.cpp
		src/Items/Sprite/SpriteWidget.cpp
		src/Items/Sprite/SpriteDraw.h
		src/Items/Sprite/SpriteModels.h
		src/Items/Sprite/SpriteTableView.h
		src/Items/Sprite/SpriteUndoCmds.h
		src/Items/Sprite/SpriteWidget.h
		src/Items/Sprite/SpriteWidget.ui
		src/Items/Text/TextDraw.cpp
		src/Items/Text/TextModel.cpp
		src/Items/Text/TextUndoCmds.cpp
		src/Items/Text/TextWidget.cpp
		src/Items/Text/TextDraw.h
		src/Items/Text/TextModel.h
		src/Items/Text/TextUndoCmds.h
		src/Items/Text/TextWidget.h
		src/Items/Text/TextWidget.ui
		src/Items/Text/SubWidgets/TextFontManager.cpp
		src/Items/Text/SubWidgets/TextLayersModel.cpp
		src/Items/Text/SubWidgets/TextLayersWidget.cpp
		src/Items/Text/SubWidgets/TextFontManager.h
		src/Items/Text/SubWidgets/TextLayersModel.h
		src/Items/Text/SubWidgets/TextLayersWidget.h
		src/Items/TileMap/TileMapDraw.cpp
		src/Items/TileMap/TileMapModels.cpp
		src/Items/TileMap/TileMapUndoCmds.cpp
		src/Items/TileMap/TileMapWidget.cpp
		src/Items/TileMap/TileMapDraw.h
		src/Items/TileMap/TileMapModels.h
		src/Items/TileMap/TileMapUndoCmds.h
		src/Items/TileMap/TileMapWidget.h
		src/Items/TileMap/TileMapWidget.ui
		src/Items/Entity/EntityDopeSheetScene.cpp
		src/Items/Entity/EntityDopeSheetScene.h
		src/Items/Entity/EntityDopeSheetView.cpp
		src/Items/Entity/EntityDopeSheetView.h
		src/Items/Entity/EntityDraw.cpp
		src/Items/Entity/EntityDraw.h
		src/Items/Entity/EntityDrawItem.cpp
		src/Items/Entity/EntityDrawItem.h
		src/Items/Entity/EntityItemMimeData.cpp
		src/Items/Entity/EntityItemMimeData.h
		src/Items/Entity/EntityModel.cpp
		src/Items/Entity/EntityModel.h
		src/Items/Entity/EntityTreeModel.cpp
		src/Items/Entity/EntityTreeModel.h
		src/Items/Entity/EntityUndoCmds.cpp
		src/Items/Entity/EntityUndoCmds.h
		src/Items/Entity/EntityWidget.cpp
		src/Items/Entity/EntityWidget.h
		src/Items/Entity/EntityWidget.ui
		src/Items/Prefab/PrefabDraw.cpp
		src/Items/Prefab/PrefabModel.cpp
		src/Items/Prefab/PrefabWidget.cpp
		src/Items/Prefab/PrefabDraw.h
		src/Items/Prefab/PrefabModel.h
		src/Items/Prefab/PrefabWidget.h
		src/Items/Prefab/PrefabWidget.ui
		src/Items/Audio/AudioDraw.cpp
		src/Items/Audio/AudioDraw.h
		src/Items/Audio/AudioModel.cpp
		src/Items/Audio/AudioModel.h
		src/Items/Audio/AudioUndoCmd.h
		src/Items/Audio/AudioUndoCmd.cpp
		src/Items/Audio/AudioPlayListModel.cpp
		src/Items/Audio/AudioPlayListModel.h
		src/Items/Audio/AudioWidget.cpp
		src/Items/Audio/AudioWidget.h
		src/Items/Audio/AudioWidget.ui
		src/Dialogs/DlgAssetProperties.cpp
		src/Dialogs/DlgAssetProperties.h
		src/Dialogs/DlgAssetProperties.ui
		src/Dialogs/DlgAtlasGroupSettings.cpp
		src/Dialogs/DlgAtlasGroupSettings.h
		src/Dialogs/DlgAtlasGroupSettings.ui
		src/Dialogs/DlgColorPicker.cpp
		src/Dialogs/DlgColorPicker.h
		src/Dialogs/DlgColorPicker.ui
		src/Dialogs/DlgInputName.cpp
		src/Dialogs/DlgInputName.h
		src/Dialogs/DlgInputName.ui
		src/Dialogs/DlgInputNumber.cpp
		src/Dialogs/DlgInputNumber.h
		src/Dialogs/DlgInputNumber.ui
		src/Dialogs/DlgSetEngineLocation.cpp
		src/Dialogs/DlgSetEngineLocation.h
		src/Dialogs/DlgSetEngineLocation.ui
		src/Dialogs/DlgSetSpineAnims.cpp
		src/Dialogs/DlgSetSpineAnims.h
		src/Dialogs/DlgSetSpineAnims.ui
		src/Dialogs/DlgSetUiPanel.cpp
		src/Dialogs/DlgSetUiPanel.h
		src/Dialogs/DlgSetUiPanel.ui
		src/Dialogs/DlgSnappingSettings.cpp
		src/Dialogs/DlgSnappingSettings.h
		src/Dialogs/DlgSnappingSettings.ui
		src/Dialogs/DlgSyncAssets.cpp
		src/Dialogs/DlgSyncAssets.h
		src/Dialogs/DlgSyncAssets.ui
		src/Dialogs/DlgTabCycle.cpp
		src/Dialogs/DlgTabCycle.h
		src/Dialogs/DlgTabCycle.ui
		src/Explorer/DlgNewProject.cpp
		src/Explorer/DlgNewProject.h
		src/Explorer/DlgNewProject.ui
		src/Explorer/ExplorerItemData.cpp
		src/Explorer/ExplorerModel.cpp
		src/Explorer/ExplorerWidget.cpp
		src/Explorer/ExplorerItemData.h
		src/Explorer/ExplorerModel.h
		src/Explorer/ExplorerWidget.h
		src/Explorer/ExplorerWidget.ui
		src/Explorer/Project/DlgCollisionFilter.cpp
		src/Explorer/Project/DlgCollisionFilter.h
		src/Explorer/Project/DlgCollisionFilter.ui
		src/Explorer/Project/DlgNewProjectItem.cpp
		src/Explorer/Project/DlgNewProjectItem.h
		src/Explorer/Project/DlgNewProjectItem.ui
		src/Explorer/Project/DlgProjectSettings.cpp
		src/Explorer/Project/DlgProjectSettings.h
		src/Explorer/Project/DlgProjectSettings.ui
		src/Explorer/Project/DlgSurfaceMaterials.cpp
		src/Explorer/Project/DlgSurfaceMaterials.h
		src/Explorer/Project/DlgSurfaceMaterials.ui
		src/Explorer/Project/Project.cpp
		src/Explorer/Project/ProjectDraw.cpp
		src/Explorer/Project/ProjectItemData.cpp
		src/Explorer/Project/ProjectItemMimeData.cpp
		src/Explorer/Project/Project.h
		src/Explorer/Project/ProjectDraw.h
		src/Explorer/Project/ProjectItemData.h
		src/Explorer/Project/ProjectItemMimeData.h
		src/Explorer/Project/SurfaceMaterialsModel.cpp
		src/Explorer/Project/SurfaceMaterialsModel.h
		src/Explorer/Project/WgtWindowInfo.cpp
		src/Explorer/Project/WgtWindowInfo.h
		src/Explorer/Project/WgtWindowInfo.ui
		src/Harmony/Harmony.cpp
		src/Harmony/HarmonyConnection.cpp
		src/Harmony/Harmony.h
		src/Harmony/HarmonyConnection.h
		src/Harmony/HarmonyRulerGfxView.cpp
		src/Harmony/HarmonyRulerGfxView.h
		src/Harmony/HarmonyWidget.cpp
		src/Harmony/HarmonyWidget.h
		src/Harmony/HarmonyWidget.ui
		src/Gfx/CommonGfxView.cpp
		src/Gfx/CommonGfxView.h
		src/Gfx/GfxGrabPointModel.cpp
		src/Gfx/GfxGrabPointModel.h
		src/Gfx/GfxGrabPointView.cpp
		src/Gfx/GfxGrabPointView.h
		src/Gfx/GfxMarqueeCtrl.cpp
		src/Gfx/GfxMarqueeCtrl.h
		src/Gfx/GfxTextModel.cpp
		src/Gfx/GfxTextModel.h
		src/Gfx/GfxTransformCtrl.cpp
		src/Gfx/GfxTransformCtrl.h
		src/Gfx/EditModels/IGfxEditModel.cpp
		src/Gfx/EditModels/IGfxEditModel.h
		src/Gfx/EditModels/IGfxEditView.cpp
		src/Gfx/EditModels/IGfxEditView.h
		src/Gfx/EditModels/Chain/GfxChainModel.cpp
		src/Gfx/EditModels/Chain/GfxChainModel.h
		src/Gfx/EditModels/Chain/GfxChainView.cpp
		src/Gfx/EditModels/Chain/GfxChainView.h
		src/Gfx/EditModels/Container/ContainerEditModel.cpp
		src/Gfx/EditModels/Container/ContainerEditModel.h
		src/Gfx/EditModels/Container/ContainerEditView.cpp
		src/Gfx/EditModels/Container/ContainerEditView.h
		src/Gfx/EditModels/Primitive/GfxPrimitiveModel.cpp
		src/Gfx/EditModels/Primitive/GfxPrimitiveModel.h
		src/Gfx/EditModels/Primitive/GfxPrimitiveView.cpp
		src/Gfx/EditModels/Primitive/GfxPrimitiveView.h
		src/Gfx/EditModels/Shape/GfxShapeModel.cpp
		src/Gfx/EditModels/Shape/GfxShapeModel.h
		src/Gfx/EditModels/Shape/GfxShapeHyView.cpp
		src/Gfx/EditModels/Shape/GfxShapeHyView.h
		src/Gfx/EditModels/Shape/GfxShapeQtView.cpp
		src/Gfx/EditModels/Shape/GfxShapeQtView.h
		src/Shared/Global.cpp
		src/Shared/Global.h
		src/Shared/GlobalUndoCmds.h
		src/Shared/GlobalWidgetMappers.h
		src/Shared/IMimeData.cpp
		src/Shared/IMimeData.h
		src/Shared/AppendWidget/WgtAppendContainer.cpp
		src/Shared/AppendWidget/WgtAppendContainer.h
		src/Shared/AppendWidget/WgtAppendContainer.ui
		src/Shared/Properties/PropertiesTreeModel.cpp
		src/Shared/Properties/PropertiesTreeModel.h
		src/Shared/Properties/PropertiesTreeMultiModel.cpp
		src/Shared/Properties/PropertiesTreeMultiModel.h
		src/Shared/Properties/PropertiesTreeView.cpp
		src/Shared/Properties/PropertiesTreeView.h
		src/Shared/Properties/PropertiesUndoCmd.cpp
		src/Shared/Properties/PropertiesUndoCmd.h
		src/Shared/TreeModel/ITreeModel.cpp
		src/Shared/TreeModel/TreeModelItem.cpp
		src/Shared/TreeModel/TreeModelItemData.cpp
		src/Shared/TreeModel/ITreeModel.h
		src/Shared/TreeModel/TreeModelItem.h
		src/Shared/TreeModel/TreeModelItemData.h
		src/Widget/Auxiliary/AuxAssetInspector.cpp
		src/Widget/Auxiliary/AuxAssetInspector.h
		src/Widget/Auxiliary/AuxAssetInspector.ui
		src/Widget/Auxiliary/AuxDopeSheet.cpp
		src/Widget/Auxiliary/AuxDopeSheet.h
		src/Widget/Auxiliary/AuxDopeSheet.ui
		src/Widget/Auxiliary/AuxOutputLog.cpp
		src/Widget/Auxiliary/AuxOutputLog.h
		src/Widget/Auxiliary/AuxOutputLog.ui
		src/Widget/Auxiliary/AuxShaderEditor.cpp
		src/Widget/Auxiliary/AuxShaderEditor.h
		src/Widget/Auxiliary/AuxShaderEditor.ui
		src/Widget/Auxiliary/AuxTileSet.cpp
		src/Widget/Auxiliary/AuxTileSet.h
		src/Widget/Auxiliary/AuxTileSet.ui
		src/Widget/WgtAudioCategory.cpp
		src/Widget/WgtAudioCategory.h
		src/Widget/WgtAudioCategory.ui
		src/Widget/WgtCodeEditor.cpp
		src/Widget/WgtCodeEditor.h
		src/Widget/WgtCodeEditor.ui
		src/Widget/WgtHarmony.cpp
		src/Widget/WgtHarmony.h
		src/Widget/WgtMakeRelDir.cpp
		src/Widget/WgtMakeRelDir.h
		src/Widget/WgtMakeRelDir.ui
		src/Widget/WgtSpineAnims.cpp
		src/Widget/WgtSpineAnims.h
		src/Widget/WgtSpineAnims.ui
		src/Widget/WgtVectorSpinBox.cpp
		src/Widget/WgtVectorSpinBox.h
		src/Widget/WgtVectorSpinBox.ui
		src/Widget/WidgetColor.cpp
		src/Widget/WidgetLiveVariable.cpp
		src/Widget/WidgetLiveVarManager.cpp
		src/Widget/WidgetRangedSlider.cpp
		src/Widget/WidgetColor.h
		src/Widget/WidgetLiveVariable.h
		src/Widget/WidgetLiveVarManager.h
		src/Widget/WidgetRangedSlider.h
		src/Widget/WidgetColor.ui
		src/Widget/WidgetLiveVariable.ui
		src/Widget/WidgetLiveVarManager.ui
		src/Widget/WidgetRangedSlider.ui
		src/_Dependencies/QtWaitingSpinner/waitingspinnerwidget.cpp
		src/_Dependencies/QtWaitingSpinner/waitingspinnerwidget.h)

	set(CMAKE_AUTOMOC ON)
	set(CMAKE_AUTORCC ON)
	set(CMAKE_AUTOUIC ON)

	set(CMAKE_INCLUDE_CURRENT_DIR ON)

	#include_directories(${Qt5Widgets_INCLUDES})
	#add_definitions(${Qt5Widgets_DEFINITIONS})

	add_executable(HyEditor WIN32 ${HYEDITOR_SOURCE_FILES} data/HyEditor.qrc)
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Assets")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Assets/Source")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Assets/Atlas")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Assets/Atlas/TileSet")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Assets/Audio")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Items")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Items/Sprite")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Items/TileMap")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Items/Text")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Items/Spine")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Items/Prefab")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Items/Primitive")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Items/Entity")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Items/Audio")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Dialogs")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Harmony")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Widget")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Widget/Auxiliary")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Explorer")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Explorer/Project")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Gfx")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Gfx/EditModels")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Gfx/EditModels/Chain")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Gfx/EditModels/Container")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Gfx/EditModels/Primitive")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Gfx/EditModels/Shape")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Shared")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Shared/Manager")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Shared/Properties")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Shared/Transform")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Shared/TreeModel")

	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../Engine/include")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../Engine/include/Harmony")

	target_compile_definitions(HyEditor PRIVATE HY_USE_QT)
	target_link_libraries(HyEditor PUBLIC HyEngine freetype-gl ogg vorbis QCodeEditor Qt6::Core Qt6::Gui Qt6::Widgets Qt6::OpenGLWidgets Qt6::Multimedia)

	set_target_properties(HyEditor PROPERTIES
		CXX_STANDARD 17
		CXX_STANDARD_REQUIRED YES
		CXX_EXTENSIONS NO)

	target_compile_definitions(HyEditor PRIVATE QT_USE_QSTRINGBUILDER QT_NO_KEYWORDS)
	target_precompile_headers(HyEditor PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/Shared/Global.h")

	# Setup windeployqt
	if (WIN32)
		get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
		get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
		find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
		
		add_custom_command(TARGET HyEditor POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E
			env PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}"
				--verbose 0
				--no-compiler-runtime
				\"$<TARGET_FILE:HyEditor>\"
		COMMENT "Deploying Qt on Windows...")
	endif ()
	
	if (MSVC)
		target_compile_definitions(HyEditor PUBLIC _CRT_SECURE_NO_WARNINGS)
	endif()

	source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${HYEDITOR_SOURCE_FILES})
else()
	message(ERROR " Required variable Qt6_DIR. Specify cmake Qt6 directory -Example- C:/Qt/6.9.3/msvc2022_64/lib/cmake/Qt6")
	message(STATUS "Press Configure again once you specify Qt6_DIR")
endif()