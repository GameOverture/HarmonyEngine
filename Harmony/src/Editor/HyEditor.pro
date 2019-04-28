#-------------------------------------------------
#
# Project created by QtCreator 2014-07-12T14:10:09
#
#-------------------------------------------------

QT += core gui opengl network widgets

TARGET = HyEditor
TEMPLATE = app

CONFIG += axcontainer
CONFIG += no_keywords

SOURCES += main.cpp \
    MainWindow.cpp \
    _Dependencies/QtWaitingSpinner/waitingspinnerwidget.cpp \
    _Dependencies/scriptum/imagecrop.cpp \
    _Dependencies/scriptum/imagepacker.cpp \
    _Dependencies/scriptum/imagesort.cpp \
    _Dependencies/scriptum/maxrects.cpp \
    Atlas/AtlasDraw.cpp \
    Atlas/AtlasFrame.cpp \
    Atlas/AtlasModel.cpp \
    Atlas/AtlasRepackThread.cpp \
    Atlas/AtlasWidget.cpp \
    Audio/AudioDraw.cpp \
    Audio/AudioModelView.cpp \
    Audio/AudioWave.cpp \
    Audio/AudioWidget.cpp \
    Audio/AudioWidgetBank.cpp \
    Audio/AudioWidgetManager.cpp \
    Audio/AudioWidgetState.cpp \
    Dialogs/DlgAtlasGroupSettings.cpp \
    Dialogs/DlgColorPicker.cpp \
    Dialogs/DlgInputName.cpp \
    Dialogs/DlgNewItem.cpp \
    Dialogs/DlgNewProject.cpp \
    Dialogs/DlgProjectSettings.cpp \
    Dialogs/DlgSetEngineLocation.cpp \
    Entity/EntityDraw.cpp \
    Entity/EntityModel.cpp \
    Entity/EntityTreeModel.cpp \
    Entity/EntityUndoCmds.cpp \
    Entity/EntityWidget.cpp \
    Entity3d/Entity3d.cpp \
    Explorer/ExplorerItem.cpp \
    Explorer/ExplorerItemMimeData.cpp \
    Explorer/ExplorerModel.cpp \
    Explorer/ExplorerWidget.cpp \
    Font/FontDraw.cpp \
    Font/FontModels.cpp \
    Font/FontUndoCmds.cpp \
    Font/FontWidget.cpp \
    Harmony/Harmony.cpp \
    Harmony/HarmonyConnection.cpp \
    Harmony/HarmonyWidget.cpp \
    Prefab/PrefabDraw.cpp \
    Prefab/PrefabModel.cpp \
    Prefab/PrefabWidget.cpp \
    Primitive/PrimitiveModel.cpp \
    Project/Project.cpp \
    Project/ProjectDraw.cpp \
    Project/ProjectItem.cpp \
    Sprite/SpriteDraw.cpp \
    Sprite/SpriteModels.cpp \
    Sprite/SpriteUndoCmds.cpp \
    Sprite/SpriteWidget.cpp \
    Widget/WidgetColor.cpp \
    Widget/WidgetGradientEditor.cpp \
    Widget/WidgetLiveVariable.cpp \
    Widget/WidgetLiveVarManager.cpp \
    Widget/WidgetOutputLog.cpp \
    Widget/WidgetRangedSlider.cpp \
    Entity/EntityTreeModel.cpp \
    Entity/EntityUndoCmds.cpp \
    Shared/Global.cpp \
    Shared/IDraw.cpp \
    Shared/IModel.cpp \
    Shared/IModelTreeItem.cpp \
    Shared/Properties/PropertiesTreeItem.cpp \
    Shared/Properties/PropertiesTreeModel.cpp \
    Shared/Properties/PropertiesTreeView.cpp \
    Shared/Properties/PropertiesUndoCmd.cpp \
    Shared/TreeModel/ITreeModel.cpp \
    Sprite/SpriteDraw.cpp \
    Sprite/SpriteModels.cpp \
    Sprite/SpriteTableView.cpp \
    Sprite/SpriteUndoCmds.cpp \
    Sprite/SpriteWidget.cpp \
    Widget/WidgetColor.cpp \
    Widget/WidgetGradientEditor.cpp \
    Widget/WidgetLiveVariable.cpp \
    Widget/WidgetLiveVarManager.cpp \
    Widget/WidgetOutputLog.cpp \
    Widget/WidgetRangedSlider.cpp \
    Widget/WidgetVectorSpinBox.cpp

HEADERS  += \
    MainWindow.h \
    _Dependencies/QtWaitingSpinner/waitingspinnerwidget.h \
    _Dependencies/scriptum/imagepacker.h \
    _Dependencies/scriptum/maxrects.h \
    Atlas/AtlasDraw.h \
    Atlas/AtlasFrame.h \
    Atlas/AtlasModel.h \
    Atlas/AtlasRepackThread.h \
    Atlas/AtlasWidget.h \
    Audio/AudioDraw.h \
    Audio/AudioModelView.h \
    Audio/AudioWave.h \
    Audio/AudioWidget.h \
    Audio/AudioWidgetBank.h \
    Audio/AudioWidgetManager.h \
    Audio/AudioWidgetState.h \
    Dialogs/DlgAtlasGroupSettings.h \
    Dialogs/DlgColorPicker.h \
    Dialogs/DlgInputName.h \
    Dialogs/DlgNewItem.h \
    Dialogs/DlgNewProject.h \
    Dialogs/DlgProjectSettings.h \
    Dialogs/DlgSetEngineLocation.h \
    Entity/EntityDraw.h \
    Entity/EntityModel.h \
    Entity/EntityTreeModel.h \
    Entity/EntityUndoCmds.h \
    Entity/EntityWidget.h \
    Entity3d/Entity3d.h \
    Explorer/ExplorerItem.h \
    Explorer/ExplorerItemMimeData.h \
    Explorer/ExplorerModel.h \
    Explorer/ExplorerWidget.h \
    Font/FontDraw.h \
    Font/FontModels.h \
    Font/FontUndoCmds.h \
    Font/FontWidget.h \
    Harmony/Harmony.h \
    Harmony/HarmonyConnection.h \
    Harmony/HarmonyWidget.h \
    Prefab/PrefabDraw.h \
    Prefab/PrefabModel.h \
    Prefab/PrefabWidget.h \
    Primitive/PrimitiveModel.h \
    Project/Project.h \
    Project/ProjectDraw.h \
    Project/ProjectItem.h \
    Shared/Global.h \
    Shared/GlobalUndoCmds.h \
    Shared/GlobalWidgetMappers.h \
    Shared/IDraw.h \
    Shared/IModel.h \
    Shared/IModelTreeItem.h \
    Shared/Properties/PropertiesTreeItem.h \
    Shared/Properties/PropertiesTreeModel.h \
    Shared/Properties/PropertiesTreeView.h \
    Shared/TreeModel/ITreeModel.h \
    Sprite/SpriteDraw.h \
    Sprite/SpriteModels.h \
    Sprite/SpriteTableView.h \
    Sprite/SpriteUndoCmds.h \
    Sprite/SpriteWidget.h \
    Widget/WidgetColor.h \
    Widget/WidgetGradientEditor.h \
    Widget/WidgetLiveVariable.h \
    Widget/WidgetLiveVarManager.h \
    Widget/WidgetOutputLog.h \
    Widget/WidgetRangedSlider.h \
    Widget/WidgetVectorSpinBox.h \

FORMS    += \
    MainWindow.ui \
    Atlas/AtlasWidget.ui \
    Audio/AudioWidget.ui \
    Audio/AudioWidgetBank.ui \
    Audio/AudioWidgetManager.ui \
    Audio/AudioWidgetState.ui \
    Dialogs/DlgAtlasGroupSettings.ui \
    Dialogs/DlgColorPicker.ui \
    Dialogs/DlgInputName.ui \
    Dialogs/DlgNewItem.ui \
    Dialogs/DlgNewProject.ui \
    Dialogs/DlgProjectSettings.ui \
    Dialogs/DlgSetEngineLocation.ui \
    Entity/EntityCommon.ui \
    Entity/EntityToolbox.ui \
    Entity/EntityWidget.ui \
    Entity/EntSpriteFrame.ui \
    Entity3d/Entity3d.ui \
    Explorer/ExplorerWidget.ui \
    Font/FontWidget.ui \
    Prefab/PrefabWidget.ui \
    Sprite/SpriteWidget.ui \
    Widget/WidgetColor.ui \
    Widget/WidgetGradientEditor.ui \
    Widget/WidgetLiveVariable.ui \
    Widget/WidgetLiveVarManager.ui \
    Widget/WidgetOutputLog.ui \
    Widget/WidgetRangedSlider.ui \
    Widget/WidgetVectorSpinBox.ui

RESOURCES += \
	res/HyEditor.qrc

RC_FILE = HyEditor.rc

DEFINES += QT_USE_QSTRINGBUILDER HY_PLATFORM_GUI

INCLUDEPATH += "$$PWD/Atlas"
INCLUDEPATH += "$$PWD/Audio"
INCLUDEPATH += "$$PWD/Dialogs"
INCLUDEPATH += "$$PWD/Entity"
INCLUDEPATH += "$$PWD/Font"
INCLUDEPATH += "$$PWD/Harmony"
INCLUDEPATH += "$$PWD/Project"
INCLUDEPATH += "$$PWD/Sprite"
INCLUDEPATH += "$$PWD/Prefab"
INCLUDEPATH += "$$PWD/Primitive"
INCLUDEPATH += "$$PWD/Widget"
INCLUDEPATH += "$$PWD/Explorer"
INCLUDEPATH += "$$PWD/Shared"
INCLUDEPATH += "$$PWD/Shared/Properties"
INCLUDEPATH += "$$PWD/Shared/TreeModel"
INCLUDEPATH += "$$PWD/../../include"
INCLUDEPATH += "$$PWD/../../include/Harmony"
DEPENDPATH += "$$PWD/../../include"

win32-g++:CONFIG(release, debug|release): {
	PRE_TARGETDEPS += $$PWD/../../lib/x64_GuiRelease/libfreetype-gl.a \
					  $$PWD/../../lib/x64_GuiRelease/libassimp.a \
					  $$PWD/../../bin/libHyEngine_x64GuiRelease.a
}
win32-g++:CONFIG(debug, debug|release): {
	PRE_TARGETDEPS += $$PWD/../../lib/x64_GuiDebug/libfreetype-gl.a \
					  $$PWD/../../lib/x64_GuiDebug/libassimp.a \
					  $$PWD/../../bin/libHyEngine_x64GuiDebug.a
}
win32-msvc*:CONFIG(release, debug|release): {
	PRE_TARGETDEPS += $$PWD/../../lib/x64_GuiRelease/freetype-gl.lib \
					  $$PWD/../../lib/x64_GuiRelease/assimp.lib \
					  $$PWD/../../bin/HyEngine_x64GuiRelease.lib
}
win32-msvc*:CONFIG(debug, debug|release): {
	PRE_TARGETDEPS += $$PWD/../../lib/x64_GuiDebug/freetype-gl.lib \
					  $$PWD/../../lib/x64_GuiDebug/assimp.lib \
					  $$PWD/../../bin/HyEngine_x64GuiDebug.lib
}

win32:CONFIG(release, debug|release): {
	LIBS += -L$$PWD/../../lib/x64_GuiRelease/ -lfreetype-gl \
			-L$$PWD/../../lib/x64_GuiRelease/ -lassimp \
			-L$$PWD/../../bin/ -lHyEngine_x64GuiRelease
}
win32:CONFIG(debug, debug|release): {
	LIBS += -L$$PWD/../../lib/x64_GuiDebug/ -lfreetype-gl \
			-L$$PWD/../../lib/x64_GuiDebug/ -lassimp \
			-L$$PWD/../../bin/ -lHyEngine_x64GuiDebug
}

win32: LIBS += -lAdvAPI32
win32: LIBS += -lole32
