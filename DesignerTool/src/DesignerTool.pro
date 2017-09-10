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
    HyGuiRenderer.cpp \
    HyGuiDebugger.cpp \
    IDraw.cpp \
    IModel.cpp \
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
    Entity/EntSpriteFrame.cpp \
    Font/FontDraw.cpp \
    Font/FontItem.cpp \
    Font/FontModels.cpp \
    Font/FontModelView.cpp \
    Font/FontUndoCmds.cpp \
    Font/FontWidget.cpp \
    Project/DataExplorerItem.cpp \
    Project/Project.cpp \
    Project/ProjectDraw.cpp \
    Project/ProjectItem.cpp \
    Sprite/SpriteDraw.cpp \
    Sprite/SpriteModels.cpp \
    Sprite/SpriteModelView.cpp \
    Sprite/SpriteUndoCmds.cpp \
    Sprite/SpriteWidget.cpp \
    Widget/WidgetColor.cpp \
    Widget/WidgetGradientEditor.cpp \
    Widget/WidgetLiveVariable.cpp \
    Widget/WidgetLiveVarManager.cpp \
    Widget/WidgetOutputLog.cpp \
    Widget/WidgetRangedSlider.cpp \
    Entity/EntityToolbox.cpp \
    Entity/EntityTreeModel.cpp \
    Project/ProjectItemMimeData.cpp \
    Project/ProjectWidget.cpp \
    Explorer/ExplorerTreeWidget.cpp \
    Entity/EntityCommon.cpp \
    Entity/EntityUndoCmds.cpp \
    Global/Global.cpp

HEADERS  += \
    MainWindow.h \
    HyGuiRenderer.h \
    HyGuiDebugger.h \
    IDraw.h \
    IModel.h \
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
    Entity/EntSpriteFrame.h \
    Font/FontDraw.h \
    Font/FontItem.h \
    Font/FontModels.h \
    Font/FontModelView.h \
    Font/FontUndoCmds.h \
    Font/FontWidget.h \
    Project/DataExplorerItem.h \
    Project/Project.h \
    Project/ProjectDraw.h \
    Project/ProjectItem.h \
    Sprite/SpriteDraw.h \
    Sprite/SpriteModels.h \
    Sprite/SpriteModelView.h \
    Sprite/SpriteUndoCmds.h \
    Sprite/SpriteWidget.h \
    Widget/WidgetColor.h \
    Widget/WidgetGradientEditor.h \
    Widget/WidgetLiveVariable.h \
    Widget/WidgetLiveVarManager.h \
    Widget/WidgetOutputLog.h \
    Widget/WidgetRangedSlider.h \
    Entity/EntityToolbox.h \
    Entity/EntityTreeModel.h \
    Project/ProjectItemMimeData.h \
    Project/ProjectWidget.h \
    Explorer/ExplorerTreeWidget.h \
    Entity/EntityCommon.h \
    Entity/EntityUndoCmds.h \
    Global/GlobalWidgetMappers.h \
    Global/Global.h \
    Global/GlobalUndoCmds.h

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
    Project/ProjectWidget.ui \
    Entity/EntityCommon.ui

RC_FILE = DesignerTool.rc

DEFINES += _HARMONYGUI _HARMONYSINGLETHREAD QT_USE_QSTRINGBUILDER HY_PLATFORM_GUI

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../Harmony/bin/x64/ -lHarmonyQt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Harmony/bin/x64/ -lHarmonyQtd

INCLUDEPATH += "$$PWD/Atlas"
INCLUDEPATH += "$$PWD/Audio"
INCLUDEPATH += "$$PWD/Dialogs"
INCLUDEPATH += "$$PWD/Entity"
INCLUDEPATH += "$$PWD/Font"
INCLUDEPATH += "$$PWD/Project"
INCLUDEPATH += "$$PWD/Sprite"
INCLUDEPATH += "$$PWD/Widget"
INCLUDEPATH += "$$PWD/Explorer"
INCLUDEPATH += "$$PWD/Global"

INCLUDEPATH += "$$PWD/../../Harmony/include"
DEPENDPATH += "$$PWD/../../Harmony/include"

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../Harmony/bin/x64/libHarmonyQt.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../Harmony/bin/x64/libHarmonyQtd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../Harmony/bin/x64/HarmonyQt.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../Harmony/bin/x64/HarmonyQtd.lib

win32: LIBS += -lAdvAPI32
win32: LIBS += -lole32

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../Harmony/lib/x64/ -lfreetype-glQt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Harmony/lib/x64/ -lfreetype-glQtd

RESOURCES += \
    res/DesignerTool.qrc
