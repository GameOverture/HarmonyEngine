#set(Qt5_DIR C:/Soft/Qt/5.12.0/msvc2017_64/lib/cmake/Qt5)
set(Qt5_DIR "" CACHE PATH "Specify cmake Qt5 directory. Ex. C:/Qt/5.12.0/msvc2017_64/lib/cmake/Qt5")

if(Qt5_DIR)
	add_subdirectory(external/freetype2)
	add_subdirectory(external/freetype-gl)

	find_package(Qt5 COMPONENTS Core Gui Network Widgets Multimedia REQUIRED)

	set(HYEDITOR_SOURCE_FILES
		src/main.cpp
		src/MainWindow.cpp
		src/VersionPatcher.cpp
		src/MainWindow.h
		src/VersionPatcher.h
		src/MainWindow.ui
		src/HyEditor.rc
		src/Atlas/AtlasDraw.cpp
		src/Atlas/AtlasFrame.cpp
		src/Atlas/AtlasModel.cpp
		src/Atlas/AtlasRepackThread.cpp
		src/Atlas/AtlasDraw.h
		src/Atlas/AtlasFrame.h
		src/Atlas/AtlasModel.h
		src/Atlas/AtlasRepackThread.h
		src/Audio/AudioAsset.cpp
		src/Audio/AudioAssetsWidget.cpp
		src/Audio/AudioManagerModel.cpp
		src/Audio/AudioModel.cpp
		src/Audio/AudioWidget.cpp
		src/Audio/AudioAsset.h
		src/Audio/AudioAssetsWidget.h
		src/Audio/AudioManagerModel.h
		src/Audio/AudioModel.h
		src/Audio/AudioWidget.h
		src/Audio/AudioAssetsWidget.ui
		src/Audio/AudioWidget.ui
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
		src/Dialogs/DlgNewProject.cpp
		src/Dialogs/DlgNewProject.h
		src/Dialogs/DlgNewProject.ui
		src/Dialogs/DlgProjectSettings.cpp
		src/Dialogs/DlgProjectSettings.h
		src/Dialogs/DlgProjectSettings.ui
		src/Dialogs/DlgSetEngineLocation.cpp
		src/Dialogs/DlgSetEngineLocation.h
		src/Dialogs/DlgSetEngineLocation.ui
		src/Entity/EntityDraw.cpp
		src/Entity/EntityModel.cpp
		src/Entity/EntityUndoCmds.cpp
		src/Entity/EntityWidget.cpp
		src/Entity/EntityDraw.h
		src/Entity/EntityModel.h
		src/Entity/EntityUndoCmds.h
		src/Entity/EntityWidget.h
		src/Entity/EntityToolbox.ui
		src/Entity/EntityWidget.ui
		src/Explorer/ExplorerItemData.cpp
		src/Explorer/ExplorerModel.cpp
		src/Explorer/ExplorerWidget.cpp
		src/Explorer/ExplorerItemData.h
		src/Explorer/ExplorerModel.h
		src/Explorer/ExplorerWidget.h
		src/Explorer/ExplorerWidget.ui
		src/Gltf/GltfModel.cpp
		src/Gltf/GltfWidget.cpp
		src/Gltf/GltfModel.h
		src/Gltf/GltfWidget.h
		src/Gltf/GltfWidget.ui
		src/Harmony/Harmony.cpp
		src/Harmony/HarmonyConnection.cpp
		src/Harmony/HarmonyWidget.cpp
		src/Harmony/Harmony.h
		src/Harmony/HarmonyConnection.h
		src/Harmony/HarmonyWidget.h
		src/Prefab/PrefabDraw.cpp
		src/Prefab/PrefabModel.cpp
		src/Prefab/PrefabWidget.cpp
		src/Prefab/PrefabDraw.h
		src/Prefab/PrefabModel.h
		src/Prefab/PrefabWidget.h
		src/Prefab/PrefabWidget.ui
		src/Primitive/PrimitiveModel.cpp
		src/Primitive/PrimitiveModel.h
		src/Project/Project.cpp
		src/Project/ProjectDraw.cpp
		src/Project/ProjectItemData.cpp
		src/Project/ProjectItemMimeData.cpp
		src/Project/Project.h
		src/Project/ProjectDraw.h
		src/Project/ProjectItemData.h
		src/Project/ProjectItemMimeData.h
		src/Shared/Global.cpp
		src/Shared/Global.h
		src/Shared/GlobalUndoCmds.h
		src/Shared/GlobalWidgetMappers.h
		src/Shared/Item/IDraw.cpp
		src/Shared/Item/IModel.cpp
		src/Shared/Item/IWidget.cpp
		src/Shared/Item/IDraw.h
		src/Shared/Item/IModel.h
		src/Shared/Item/IWidget.h
		src/Shared/Item/IWidget.ui
		src/Shared/Manager/AssetMimeData.cpp
		src/Shared/Manager/AssetMimeData.h
		src/Shared/Manager/BanksModel.h
		src/Shared/Manager/BanksModel.cpp
		src/Shared/Manager/IAssetItemData.cpp
		src/Shared/Manager/IAssetItemData.h
		src/Shared/Manager/IManagerDraw.cpp
		src/Shared/Manager/IManagerDraw.h
		src/Shared/Manager/IManagerModel.cpp
		src/Shared/Manager/IManagerModel.h
		src/Shared/Manager/ManagerWidget.cpp
		src/Shared/Manager/ManagerWidget.h
		src/Shared/Manager/ManagerWidget.ui
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
		src/Sprite/SpriteDraw.cpp
		src/Sprite/SpriteModels.cpp
		src/Sprite/SpriteTableView.cpp
		src/Sprite/SpriteUndoCmds.cpp
		src/Sprite/SpriteWidget.cpp
		src/Sprite/SpriteDraw.h
		src/Sprite/SpriteModels.h
		src/Sprite/SpriteTableView.h
		src/Sprite/SpriteUndoCmds.h
		src/Sprite/SpriteWidget.h
		src/Sprite/SpriteWidget.ui
		src/Text/TextDraw.cpp
		src/Text/TextModel.cpp
		src/Text/TextUndoCmds.cpp
		src/Text/TextWidget.cpp
		src/Text/TextDraw.h
		src/Text/TextModel.h
		src/Text/TextUndoCmds.h
		src/Text/TextWidget.h
		src/Text/TextWidget.ui
		src/Text/SubWidgets/TextFontManager.cpp
		src/Text/SubWidgets/TextLayersModel.cpp
		src/Text/SubWidgets/TextLayersWidget.cpp
		src/Text/SubWidgets/TextFontManager.h
		src/Text/SubWidgets/TextLayersModel.h
		src/Text/SubWidgets/TextLayersWidget.h
		src/Widget/WgtMakeRelDir.cpp
		src/Widget/WgtMakeRelDir.h
		src/Widget/WgtMakeRelDir.ui
		src/Widget/WidgetColor.cpp
		src/Widget/WidgetGradientEditor.cpp
		src/Widget/WidgetLiveVariable.cpp
		src/Widget/WidgetLiveVarManager.cpp
		src/Widget/WidgetOutputLog.cpp
		src/Widget/WidgetRangedSlider.cpp
		src/Widget/WidgetVectorSpinBox.cpp
		src/Widget/WidgetColor.h
		src/Widget/WidgetGradientEditor.h
		src/Widget/WidgetLiveVariable.h
		src/Widget/WidgetLiveVarManager.h
		src/Widget/WidgetOutputLog.h
		src/Widget/WidgetRangedSlider.h
		src/Widget/WidgetVectorSpinBox.h
		src/Widget/WidgetColor.ui
		src/Widget/WidgetGradientEditor.ui
		src/Widget/WidgetLiveVariable.ui
		src/Widget/WidgetLiveVarManager.ui
		src/Widget/WidgetOutputLog.ui
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

	add_executable(HyEditor ${HYEDITOR_SOURCE_FILES} data/HyEditor.qrc)
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Atlas")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Audio")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Dialogs")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Entity")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Text")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Harmony")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Project")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Sprite")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Prefab")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Primitive")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Widget")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Explorer")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Gltf")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Shared")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Shared/Item")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Shared/Manager")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Shared/Properties")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Shared/TreeModel")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../Engine/include")
	target_include_directories(HyEditor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../Engine/include/Harmony")

	target_compile_definitions(HyEditor PRIVATE HY_USE_QT)
	target_link_libraries(HyEditor PUBLIC HyEngine freetype-gl Qt5::Core Qt5::Gui Qt5::Network Qt5::Widgets Qt5::Multimedia)

	set_target_properties(HyEditor PROPERTIES
		CXX_STANDARD 11
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

	# Sets up filters for VS
	source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" FILES ${HYEDITOR_SOURCE_FILES})
else()
	message(WARNING "Required variable Qt5_DIR. Specify cmake Qt5 directory -Example- C:/Qt/5.12.0/msvc2017_64/lib/cmake/Qt5")
endif()