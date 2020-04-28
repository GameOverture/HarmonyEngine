set(HYEDITOR_SOURCE_FILES
	main.cpp
	MainWindow.cpp
	VersionPatcher.cpp
	MainWindow.h
	VersionPatcher.h
	MainWindow.ui
	HyEditor.rc
	Atlas/AtlasDraw.cpp
	Atlas/AtlasFrame.cpp
	Atlas/AtlasModel.cpp
	Atlas/AtlasRepackThread.cpp
	Atlas/AtlasWidget.cpp
	Atlas/AtlasDraw.h
	Atlas/AtlasFrame.h
	Atlas/AtlasModel.h
	Atlas/AtlasRepackThread.h
	Atlas/AtlasWidget.h
	Atlas/AtlasWidget.ui
	Audio/AudioAssetsWidget.cpp
	Audio/AudioModel.cpp
	Audio/AudioWidget.cpp
	Audio/AudioAssetsWidget.h
	Audio/AudioModel.h
	Audio/AudioWidget.h
	Audio/AudioAssetsWidget.ui
	Audio/AudioWidget.ui
	Dialogs/DlgAtlasGroupSettings.cpp
	Dialogs/DlgColorPicker.cpp
	Dialogs/DlgInputName.cpp
	Dialogs/DlgNewItem.cpp
	Dialogs/DlgNewProject.cpp
	Dialogs/DlgProjectSettings.cpp
	Dialogs/DlgSetEngineLocation.cpp
	Dialogs/DlgAtlasGroupSettings.h
	Dialogs/DlgColorPicker.h
	Dialogs/DlgInputName.h
	Dialogs/DlgNewItem.h
	Dialogs/DlgNewProject.h
	Dialogs/DlgProjectSettings.h
	Dialogs/DlgSetEngineLocation.h
	Dialogs/DlgAtlasGroupSettings.ui
	Dialogs/DlgColorPicker.ui
	Dialogs/DlgInputName.ui
	Dialogs/DlgNewItem.ui
	Dialogs/DlgNewProject.ui
	Dialogs/DlgProjectSettings.ui
	Dialogs/DlgSetEngineLocation.ui
	Entity/EntityDraw.cpp
	Entity/EntityModel.cpp
	Entity/EntityUndoCmds.cpp
	Entity/EntityWidget.cpp
	Entity/EntityDraw.h
	Entity/EntityModel.h
	Entity/EntityUndoCmds.h
	Entity/EntityWidget.h
	Entity/EntityToolbox.ui
	Entity/EntityWidget.ui
	Explorer/ExplorerItemData.cpp
	Explorer/ExplorerModel.cpp
	Explorer/ExplorerWidget.cpp
	Explorer/ExplorerItemData.h
	Explorer/ExplorerModel.h
	Explorer/ExplorerWidget.h
	Explorer/ExplorerWidget.ui
	Gltf/GltfModel.cpp
	Gltf/GltfWidget.cpp
	Gltf/GltfModel.h
	Gltf/GltfWidget.h
	Gltf/GltfWidget.ui
	Harmony/Harmony.cpp
	Harmony/HarmonyConnection.cpp
	Harmony/HarmonyWidget.cpp
	Harmony/Harmony.h
	Harmony/HarmonyConnection.h
	Harmony/HarmonyWidget.h
	Prefab/PrefabDraw.cpp
	Prefab/PrefabModel.cpp
	Prefab/PrefabWidget.cpp
	Prefab/PrefabDraw.h
	Prefab/PrefabModel.h
	Prefab/PrefabWidget.h
	Prefab/PrefabWidget.ui
	Primitive/PrimitiveModel.cpp
	Primitive/PrimitiveModel.h
	Project/Project.cpp
	Project/ProjectDraw.cpp
	Project/ProjectItemData.cpp
	Project/ProjectItemMimeData.cpp
	Project/Project.h
	Project/ProjectDraw.h
	Project/ProjectItemData.h
	Project/ProjectItemMimeData.h
	Shared/Global.cpp
	Shared/IDraw.cpp
	Shared/IModel.cpp
	Shared/IWidget.cpp
	Shared/Global.h
	Shared/GlobalUndoCmds.h
	Shared/GlobalWidgetMappers.h
	Shared/IDraw.h
	Shared/IModel.h
	Shared/IWidget.h
	Shared/IWidget.ui
	Shared/Properties/PropertiesTreeModel.cpp
	Shared/Properties/PropertiesTreeView.cpp
	Shared/Properties/PropertiesUndoCmd.cpp
	Shared/Properties/PropertiesTreeModel.h
	Shared/Properties/PropertiesTreeView.h
	Shared/Properties/PropertiesUndoCmd.h
	Shared/TreeModel/ITreeModel.cpp
	Shared/TreeModel/TreeModelItem.cpp
	Shared/TreeModel/TreeModelItemData.cpp
	Shared/TreeModel/ITreeModel.h
	Shared/TreeModel/TreeModelItem.h
	Shared/TreeModel/TreeModelItemData.h
	Sprite/SpriteDraw.cpp
	Sprite/SpriteModels.cpp
	Sprite/SpriteTableView.cpp
	Sprite/SpriteUndoCmds.cpp
	Sprite/SpriteWidget.cpp
	Sprite/SpriteDraw.h
	Sprite/SpriteModels.h
	Sprite/SpriteTableView.h
	Sprite/SpriteUndoCmds.h
	Sprite/SpriteWidget.h
	Sprite/SpriteWidget.ui
	Text/TextDraw.cpp
	Text/TextModel.cpp
	Text/TextUndoCmds.cpp
	Text/TextWidget.cpp
	Text/TextDraw.h
	Text/TextModel.h
	Text/TextUndoCmds.h
	Text/TextWidget.h
	Text/TextWidget.ui
	Text/SubWidgets/TextFontManager.cpp
	Text/SubWidgets/TextLayersModel.cpp
	Text/SubWidgets/TextLayersWidget.cpp
	Text/SubWidgets/TextFontManager.h
	Text/SubWidgets/TextLayersModel.h
	Text/SubWidgets/TextLayersWidget.h
	Widget/WidgetColor.cpp
	Widget/WidgetGradientEditor.cpp
	Widget/WidgetLiveVariable.cpp
	Widget/WidgetLiveVarManager.cpp
	Widget/WidgetOutputLog.cpp
	Widget/WidgetRangedSlider.cpp
	Widget/WidgetVectorSpinBox.cpp
	Widget/WidgetColor.h
	Widget/WidgetGradientEditor.h
	Widget/WidgetLiveVariable.h
	Widget/WidgetLiveVarManager.h
	Widget/WidgetOutputLog.h
	Widget/WidgetRangedSlider.h
	Widget/WidgetVectorSpinBox.h
	Widget/WidgetColor.ui
	Widget/WidgetGradientEditor.ui
	Widget/WidgetLiveVariable.ui
	Widget/WidgetLiveVarManager.ui
	Widget/WidgetOutputLog.ui
	Widget/WidgetRangedSlider.ui
	Widget/WidgetVectorSpinBox.ui
	_Dependencies/QtWaitingSpinner/waitingspinnerwidget.cpp
	_Dependencies/QtWaitingSpinner/waitingspinnerwidget.h
	_Dependencies/scriptum/imagecrop.cpp
	_Dependencies/scriptum/imagepacker.cpp
	_Dependencies/scriptum/imagesort.cpp
	_Dependencies/scriptum/maxrects.cpp
	_Dependencies/scriptum/imagepacker.h
	_Dependencies/scriptum/maxrects.h)

set(CMAKE_PREFIX_PATH C:/Soft/Qt/5.12.0/msvc2017_64)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS GUI REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(HyEditor STATIC ${HYEDITOR_SOURCE_FILES})
target_include_directories(HyEditor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
target_include_directories(HyEditor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include/Harmony)
set_target_properties(HyEditor PROPERTIES
	CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

# Sets up filters for VS
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HYEDITOR_SOURCE_FILES})
