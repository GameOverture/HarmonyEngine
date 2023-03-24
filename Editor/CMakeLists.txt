#set(Qt5_DIR C:/Soft/Qt/5.12.0/msvc2017_64/lib/cmake/Qt5)
set(Qt5_DIR "" CACHE PATH "Specify cmake Qt5 directory. Ex. C:/Qt/5.12.0/msvc2017_64/lib/cmake/Qt5")

if(Qt5_DIR)
	add_subdirectory(vendor/freetype2)
	set_target_properties(freetype PROPERTIES FOLDER "Editor Libs")
	
	cmake_policy(SET CMP0079 NEW)
	add_subdirectory(vendor/freetype-gl)
	set_target_properties(freetype-gl PROPERTIES FOLDER "Editor Libs")
	target_include_directories(freetype-gl PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../Engine/vendor/glad/include")
	target_include_directories(freetype-gl PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/vendor/freetype2/include")
	target_link_libraries(freetype-gl PUBLIC freetype)
	
	add_subdirectory(vendor/ogg)
	set_target_properties(ogg PROPERTIES FOLDER "Editor Libs")
	
	add_subdirectory(vendor/vorbis)
	set_target_properties(vorbis PROPERTIES FOLDER "Editor Libs")
	set_target_properties(vorbisenc PROPERTIES FOLDER "Editor Libs")
	set_target_properties(vorbisfile PROPERTIES FOLDER "Editor Libs")

	find_package(Qt5 COMPONENTS Core Gui Network Widgets Multimedia REQUIRED)

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
		src/Assets/IManagerDraw.cpp
		src/Assets/IManagerDraw.h
		src/Assets/IManagerModel.cpp
		src/Assets/IManagerModel.h
		src/Assets/IRepackThread.cpp
		src/Assets/IRepackThread.h
		src/Assets/ManagerWidget.cpp
		src/Assets/ManagerWidget.h
		src/Assets/ManagerWidget.ui
		src/Assets/Source/SourceFile.cpp
		src/Assets/Source/SourceFile.h
		src/Assets/Source/SourceModel.cpp
		src/Assets/Source/SourceModel.h
		src/Assets/Source/SourceGenFileDlg.cpp
		src/Assets/Source/SourceGenFileDlg.h
		src/Assets/Source/SourceGenFileDlg.ui
		src/Assets/Source/SourceSettingsDlg.cpp
		src/Assets/Source/SourceSettingsDlg.h
		src/Assets/Source/SourceSettingsDlg.ui
		src/Assets/Atlas/AtlasDraw.cpp
		src/Assets/Atlas/AtlasDraw.h
		src/Assets/Atlas/AtlasFrame.cpp
		src/Assets/Atlas/AtlasFrame.h
		src/Assets/Atlas/AtlasModel.cpp
		src/Assets/Atlas/AtlasModel.h
		src/Assets/Atlas/AtlasRepackThread.cpp
		src/Assets/Atlas/AtlasRepackThread.h
		src/Assets/Audio/AudioAsset.cpp
		src/Assets/Audio/AudioAsset.h
		src/Assets/Audio/AudioAssetsWidget.cpp
		src/Assets/Audio/AudioAssetsWidget.h
		src/Assets/Audio/AudioAssetsWidget.ui
		src/Assets/Audio/AudioManagerModel.cpp
		src/Assets/Audio/AudioManagerModel.h
		src/Assets/Audio/AudioRepackThread.cpp
		src/Assets/Audio/AudioRepackThread.h
		src/Items/IDraw.cpp
		src/Items/IModel.cpp
		src/Items/IWidget.cpp
		src/Items/IDraw.h
		src/Items/IModel.h
		src/Items/IWidget.h
		src/Items/IWidget.ui
		src/Items/Spine/SpineDraw.cpp
		src/Items/Spine/SpineDraw.h
		src/Items/Spine/SpineModel.cpp
		src/Items/Spine/SpineModel.h
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
		src/Items/Entity/EntityDraw.cpp
		src/Items/Entity/EntityDraw.h
		src/Items/Entity/EntityDrawItem.cpp
		src/Items/Entity/EntityDrawItem.h
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
		src/Items/Primitive/PrimitiveModel.cpp
		src/Items/Primitive/PrimitiveModel.h
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
		src/Dialogs/DlgImportTileSheet.cpp
		src/Dialogs/DlgImportTileSheet.h
		src/Dialogs/DlgImportTileSheet.ui
		src/Dialogs/DlgInputName.cpp
		src/Dialogs/DlgInputName.h
		src/Dialogs/DlgInputName.ui
		src/Dialogs/DlgNewBuild.cpp
		src/Dialogs/DlgNewBuild.h
		src/Dialogs/DlgNewBuild.ui
		src/Dialogs/DlgNewItem.cpp
		src/Dialogs/DlgNewItem.h
		src/Dialogs/DlgNewItem.ui
		src/Dialogs/DlgNewPackage.cpp
		src/Dialogs/DlgNewPackage.h
		src/Dialogs/DlgNewPackage.ui
		src/Dialogs/DlgNewProject.cpp
		src/Dialogs/DlgNewProject.h
		src/Dialogs/DlgNewProject.ui
		src/Dialogs/DlgProjectSettings.cpp
		src/Dialogs/DlgProjectSettings.h
		src/Dialogs/DlgProjectSettings.ui
		src/Dialogs/DlgSetEngineLocation.cpp
		src/Dialogs/DlgSetEngineLocation.h
		src/Dialogs/DlgSetEngineLocation.ui
		src/Explorer/ExplorerItemData.cpp
		src/Explorer/ExplorerModel.cpp
		src/Explorer/ExplorerWidget.cpp
		src/Explorer/ExplorerItemData.h
		src/Explorer/ExplorerModel.h
		src/Explorer/ExplorerWidget.h
		src/Explorer/ExplorerWidget.ui
		src/Explorer/Project/Project.cpp
		src/Explorer/Project/ProjectDraw.cpp
		src/Explorer/Project/ProjectItemData.cpp
		src/Explorer/Project/ProjectItemMimeData.cpp
		src/Explorer/Project/Project.h
		src/Explorer/Project/ProjectDraw.h
		src/Explorer/Project/ProjectItemData.h
		src/Explorer/Project/ProjectItemMimeData.h
		src/Harmony/Harmony.cpp
		src/Harmony/HarmonyConnection.cpp
		src/Harmony/HarmonyWidget.cpp
		src/Harmony/Harmony.h
		src/Harmony/HarmonyConnection.h
		src/Harmony/HarmonyWidget.h
		src/Shared/Global.cpp
		src/Shared/Global.h
		src/Shared/GlobalUndoCmds.h
		src/Shared/GlobalWidgetMappers.h
		src/Shared/IMimeData.cpp
		src/Shared/IMimeData.h
		src/Shared/ShapeCtrl.cpp
		src/Shared/ShapeCtrl.h
		src/Shared/TransformCtrl.cpp
		src/Shared/TransformCtrl.h
		src/Shared/Properties/PropertiesTreeModel.cpp
		src/Shared/Properties/PropertiesTreeView.cpp
		src/Shared/Properties/PropertiesUndoCmd.cpp
		src/Shared/Properties/PropertiesTreeModel.h
		src/Shared/Properties/PropertiesTreeView.h
		src/Shared/Properties/PropertiesUndoCmd.h
		src/Shared/TreeModel/ITreeModel.cpp
		src/Shared/TreeModel/TreeModelItem.cpp
		src/Shared/TreeModel/TreeModelItemData.cpp
		src/Shared/TreeModel/ITreeModel.h
		src/Shared/TreeModel/TreeModelItem.h
		src/Shared/TreeModel/TreeModelItemData.h
		src/Widget/Auxiliary/AuxOutputLog.cpp
		src/Widget/Auxiliary/AuxOutputLog.h
		src/Widget/Auxiliary/AuxOutputLog.ui
		src/Widget/Auxiliary/AuxAssetInspector.cpp
		src/Widget/Auxiliary/AuxAssetInspector.h
		src/Widget/Auxiliary/AuxAssetInspector.ui
		src/Widget/Auxiliary/AuxToolBox.cpp
		src/Widget/Auxiliary/AuxToolBox.h
		src/Widget/Auxiliary/AuxToolBox.ui
		src/Widget/WgtAudioGroup.cpp
		src/Widget/WgtAudioGroup.h
		src/Widget/WgtAudioGroup.ui
		src/Widget/WgtMakeRelDir.cpp
		src/Widget/WgtMakeRelDir.h
		src/Widget/WgtMakeRelDir.ui
		src/Widget/WgtSrcDependency.cpp
		src/Widget/WgtSrcDependency.h
		src/Widget/WgtSrcDependency.ui
		src/Widget/WidgetColor.cpp
		src/Widget/WidgetGradientEditor.cpp
		src/Widget/WidgetLiveVariable.cpp
		src/Widget/WidgetLiveVarManager.cpp
		src/Widget/WidgetRangedSlider.cpp
		src/Widget/WidgetVectorSpinBox.cpp
		src/Widget/WidgetColor.h
		src/Widget/WidgetGradientEditor.h
		src/Widget/WidgetLiveVariable.h
		src/Widget/WidgetLiveVarManager.h
		src/Widget/WidgetRangedSlider.h
		src/Widget/WidgetVectorSpinBox.h
		src/Widget/WidgetColor.ui
		src/Widget/WidgetGradientEditor.ui
		src/Widget/WidgetLiveVariable.ui
		src/Widget/WidgetLiveVarManager.ui
		src/Widget/WidgetRangedSlider.ui
		src/Widget/WidgetVectorSpinBox.ui
		src/_Dependencies/QtWaitingSpinner/waitingspinnerwidget.cpp
		src/_Dependencies/QtWaitingSpinner/waitingspinnerwidget.h
		src/_Dependencies/scriptum/imagecrop.cpp
		src/_Dependencies/scriptum/imagepacker.cpp
		src/_Dependencies/scriptum/imagesort.cpp
		src/_Dependencies/scriptum/maxrects.cpp
		src/_Dependencies/scriptum/imagepacker.h
		src/_Dependencies/scriptum/maxrects.h)

	set(CMAKE_AUTOMOC ON)
	set(CMAKE_AUTORCC ON)
	set(CMAKE_AUTOUIC ON)

	set(CMAKE_INCLUDE_CURRENT_DIR ON)

	include_directories(${Qt5Widgets_INCLUDES})
	add_definitions(${Qt5Widgets_DEFINITIONS})

	add_executable(HyEditor WIN32 ${HYEDITOR_SOURCE_FILES} data/HyEditor.qrc)
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Assets")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Assets/Source")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Assets/Atlas")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Assets/Audio")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Items")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Items/Sprite")
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
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Shared")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Shared/Manager")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Shared/Properties")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Shared/TreeModel")

	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../Engine/include")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../Engine/include/Harmony")

	target_compile_definitions(HyEditor PRIVATE HY_USE_QT)
	target_link_libraries(HyEditor PUBLIC HyEngine freetype-gl ogg vorbis Qt5::Core Qt5::Gui Qt5::Network Qt5::Widgets Qt5::Multimedia)

	set_target_properties(HyEditor PROPERTIES
		CXX_STANDARD 17
		CXX_STANDARD_REQUIRED YES
		CXX_EXTENSIONS NO)

	target_compile_definitions(HyEditor PRIVATE QT_USE_QSTRINGBUILDER QT_NO_KEYWORDS)
	target_precompile_headers(HyEditor PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/Shared/Global.h")

	# Setup windeployqt
	if (WIN32)
		get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
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
	message(WARNING "Required variable Qt5_DIR. Specify cmake Qt5 directory -Example- C:/Qt/5.12.0/msvc2017_64/lib/cmake/Qt5")
endif()