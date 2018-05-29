#-------------------------------------------------
#
# Project created by QtCreator 2014-07-12T14:10:09
#
#-------------------------------------------------

QT       += core gui opengl network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DesignerTool
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
    Entity/EntityWidget.cpp \
    Font/FontDraw.cpp \
    Font/FontItem.cpp \
    Font/FontModels.cpp \
    Font/FontModelView.cpp \
    Font/FontUndoCmds.cpp \
    Font/FontWidget.cpp \
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
    Project/ProjectItemMimeData.cpp \
    Entity/EntityUndoCmds.cpp \
    Shared/Global.cpp \
    Shared/IDraw.cpp \
    Shared/IModel.cpp \
    Shared/Properties/PropertiesTreeView.cpp \
    Shared/Properties/PropertiesTreeItem.cpp \
    Shared/Properties/PropertiesTreeModel.cpp \
    Widget/WidgetVectorSpinBox.cpp \
    Sprite/SpriteTableView.cpp \
    Explorer/ExplorerWidget.cpp \
    Explorer/ExplorerTreeWidget.cpp \
    Harmony/Harmony.cpp \
    Harmony/HarmonyConnection.cpp \
    Harmony/HarmonyWidget.cpp \
    Atlas/AtlasRepackThread.cpp \
    Explorer/ExplorerItem.cpp \
    Shared/Properties/PropertiesUndoCmd.cpp \
    Shared/IModelTreeItem.cpp

HEADERS  += \
    MainWindow.h \
    _Dependencies/QtWaitingSpinner/waitingspinnerwidget.h \
    _Dependencies/scriptum/imagepacker.h \
    _Dependencies/scriptum/maxrects.h \
    Atlas/AtlasDraw.h \
    Atlas/AtlasFrame.h \
    Atlas/AtlasModel.h \
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
    Entity/EntityWidget.h \
    Font/FontDraw.h \
    Font/FontItem.h \
    Font/FontModels.h \
    Font/FontModelView.h \
    Font/FontUndoCmds.h \
    Font/FontWidget.h \
    Project/Project.h \
    Project/ProjectDraw.h \
    Project/ProjectItem.h \
    Sprite/SpriteDraw.h \
    Sprite/SpriteModels.h \
    Sprite/SpriteUndoCmds.h \
    Sprite/SpriteWidget.h \
    Widget/WidgetColor.h \
    Widget/WidgetGradientEditor.h \
    Widget/WidgetLiveVariable.h \
    Widget/WidgetLiveVarManager.h \
    Widget/WidgetOutputLog.h \
    Widget/WidgetRangedSlider.h \
    Entity/EntityTreeModel.h \
    Project/ProjectItemMimeData.h \
    Explorer/ExplorerTreeWidget.h \
    Entity/EntityUndoCmds.h \
    Shared/Global.h \
    Shared/GlobalUndoCmds.h \
    Shared/GlobalWidgetMappers.h \
    Shared/IDraw.h \
    Shared/IModel.h \
    Shared/IModelTreeItem.h \
    Shared/Properties/PropertiesTreeView.h \
    Shared/Properties/PropertiesTreeItem.h \
    Shared/Properties/PropertiesTreeModel.h \
    Widget/WidgetVectorSpinBox.h \
    Sprite/SpriteTableView.h \
    Explorer/ExplorerWidget.h \
    Harmony/Harmony.h \
    Harmony/HarmonyConnection.h \
    Harmony/HarmonyWidget.h \
    Atlas/AtlasRepackThread.h \
    Explorer/ExplorerItem.h \
    Shared/Properties/PropertiesUndoCmd.h

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
    Entity/EntityWidget.ui \
    Entity/EntSpriteFrame.ui \
    Font/FontWidget.ui \
    Sprite/SpriteWidget.ui \
    Widget/WidgetColor.ui \
    Widget/WidgetGradientEditor.ui \
    Widget/WidgetLiveVariable.ui \
    Widget/WidgetLiveVarManager.ui \
    Widget/WidgetOutputLog.ui \
    Widget/WidgetRangedSlider.ui \
    Entity/EntityToolbox.ui \
    Entity/EntityCommon.ui \
    Widget/WidgetVectorSpinBox.ui \
    Explorer/ExplorerWidget.ui

RC_FILE = DesignerTool.rc

DEFINES += _HARMONYGUI _HARMONYSINGLETHREAD QT_USE_QSTRINGBUILDER HY_PLATFORM_GUI

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../Harmony/bin/x64_GuiRelease/ -lHarmony
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Harmony/bin/x64_GuiDebug/ -lHarmony

INCLUDEPATH += "$$PWD/Atlas"
INCLUDEPATH += "$$PWD/Audio"
INCLUDEPATH += "$$PWD/Dialogs"
INCLUDEPATH += "$$PWD/Entity"
INCLUDEPATH += "$$PWD/Font"
INCLUDEPATH += "$$PWD/Harmony"
INCLUDEPATH += "$$PWD/Project"
INCLUDEPATH += "$$PWD/Sprite"
INCLUDEPATH += "$$PWD/Widget"
INCLUDEPATH += "$$PWD/Explorer"
INCLUDEPATH += "$$PWD/Shared"
INCLUDEPATH += "$$PWD/Shared/Properties"

INCLUDEPATH += "$$PWD/../../Harmony/include"
DEPENDPATH += "$$PWD/../../Harmony/include"

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../Harmony/bin/x64_GuiRelease/libHarmony.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../Harmony/bin/x64_GuiDebug/libHarmony.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../Harmony/bin/x64_GuiRelease/Harmony.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../Harmony/bin/x64_GuiDebug/Harmony.lib

win32: LIBS += -lAdvAPI32
win32: LIBS += -lole32

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../Harmony/lib/x64_GuiRelease/ -lfreetype-gl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Harmony/lib/x64_GuiDebug/ -lfreetype-gl

RESOURCES += \
    res/DesignerTool.qrc
