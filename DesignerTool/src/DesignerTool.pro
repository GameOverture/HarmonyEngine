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
    DlgNewProject.cpp \
    WidgetOutputLog.cpp \
    MainWindow.cpp \
    DlgNewItem.cpp \
    scriptum/imagecrop.cpp \
    scriptum/imagepacker.cpp \
    scriptum/imagesort.cpp \
    scriptum/maxrects.cpp \
    DlgInputName.cpp \
    WidgetLiveVariable.cpp \
    WidgetLiveVarManager.cpp \
    HyGuiRenderer.cpp \
    DlgAtlasGroupSettings.cpp \
    HyGuiDebugger.cpp \
    DlgSetEngineLocation.cpp \
    DlgColorPicker.cpp \
    WidgetColor.cpp \
    QtWaitingSpinner/waitingspinnerwidget.cpp \
    WidgetRangedSlider.cpp \
    WidgetGradientEditor.cpp \
    IDraw.cpp \
    SpriteDraw.cpp \
    SpriteWidget.cpp \
    FontWidget.cpp \
    FontWidgetState.cpp \
    SpriteWidgetState.cpp \
    AudioWidget.cpp \
    AudioWidgetBank.cpp \
    AudioWidgetManager.cpp \
    AudioModelView.cpp \
    AudioWidgetState.cpp \
    FontModelView.cpp \
    FontUndoCmds.cpp \
    SpriteModelView.cpp \
    SpriteUndoCmds.cpp \
    Project.cpp \
    AtlasFrame.cpp \
    AudioWave.cpp \
    HyGuiGlobal.cpp \
    AtlasesData.cpp \
    AtlasesWidget.cpp \
    ExplorerItem.cpp \
    ExplorerWidget.cpp \
    FontDraw.cpp \
    AudioDraw.cpp \
    IProjItem.cpp \
    SpriteItem.cpp \
    AudioItem.cpp \
    FontItem.cpp \
    SpriteModels.cpp

HEADERS  += \
    DlgNewProject.h \
    WidgetOutputLog.h \
    MainWindow.h \
    DlgNewItem.h \
    scriptum/imagepacker.h \
    scriptum/maxrects.h \
    DlgInputName.h \
    WidgetLiveVariable.h \
    WidgetLiveVarManager.h \
    HyGuiRenderer.h \
    DlgAtlasGroupSettings.h \
    HyGuiDebugger.h \
    DlgSetEngineLocation.h \
    DlgColorPicker.h \
    WidgetColor.h \
    QtWaitingSpinner/waitingspinnerwidget.h \
    WidgetRangedSlider.h \
    WidgetGradientEditor.h \
    IDraw.h \
    SpriteDraw.h \
    SpriteWidget.h \
    FontWidget.h \
    FontWidgetState.h \
    SpriteWidgetState.h \
    AudioWidget.h \
    UndoCmds.h \
    SpriteUndoCmds.h \
    SpriteModelView.h \
    FontUndoCmds.h \
    FontModelView.h \
    AudioWidgetState.h \
    AudioWidgetBank.h \
    AudioWidgetManager.h \
    AudioModelView.h \
    Project.h \
    AtlasFrame.h \
    AudioWave.h \
    HyGuiGlobal.h \
    AtlasesData.h \
    AtlasesWidget.h \
    ExplorerItem.h \
    ExplorerWidget.h \
    FontDraw.h \
    AudioDraw.h \
    IProjItem.h \
    SpriteItem.h \
    AudioItem.h \
    FontItem.h \
    SpriteModels.h

FORMS    += \
    DlgNewProject.ui \
    WidgetOutputLog.ui \
    MainWindow.ui \
    DlgNewItem.ui \
    DlgInputName.ui \
    WidgetLiveVariable.ui \
    WidgetLiveVarManager.ui \
    DlgAtlasGroupSettings.ui \
    DlgSetEngineLocation.ui \
    DlgColorPicker.ui \
    WidgetColor.ui \
    WidgetRangedSlider.ui \
    WidgetGradientEditor.ui \
    SpriteWidget.ui \
    FontWidget.ui \
    FontWidgetState.ui \
    SpriteWidgetState.ui \
    AudioWidget.ui \
    AudioWidgetBank.ui \
    AudioWidgetManager.ui \
    AtlasesWidget.ui \
    AudioWidgetState.ui \
    ExplorerWidget.ui

DEFINES += _HARMONYGUI _HARMONYSINGLETHREAD QT_USE_QSTRINGBUILDER HY_PLATFORM_GUI

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../Harmony/bin/x64/ -lHarmonyQt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Harmony/bin/x64/ -lHarmonyQtd

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
