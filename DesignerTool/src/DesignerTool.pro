#-------------------------------------------------
#
# Project created by QtCreator 2014-07-12T14:10:09
#
#-------------------------------------------------

QT       += core gui opengl network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DesignerTool
TEMPLATE = app

SOURCES += main.cpp \
    DlgNewProject.cpp \
    WidgetExplorer.cpp \
    WidgetOutputLog.cpp \
    MainWindow.cpp \
    HyGlobal.cpp \
    Item.cpp \
    ItemSprite.cpp \
    ItemFont.cpp \
    WidgetGlyphCreator.cpp \
    WidgetFontMap.cpp \
    DlgNewItem.cpp \
    scriptum/imagecrop.cpp \
    scriptum/imagepacker.cpp \
    scriptum/imagesort.cpp \
    scriptum/maxrects.cpp \
    WidgetSprite.cpp \
    DlgInputName.cpp \
    ItemProject.cpp \
    WidgetLiveVariable.cpp \
    WidgetLiveVarManager.cpp \
    HyGuiRenderer.cpp \
    DlgAtlasGroupSettings.cpp \
    WidgetAtlasManager.cpp \
    WidgetAtlasGroup.cpp \
    HyGuiDebugger.cpp \
    DlgSetEngineLocation.cpp \
    ItemSpriteCmds.cpp \
    WidgetSpriteState.cpp \
    HyGuiFrame.cpp \
    ItemWidget.cpp

HEADERS  += \
    DlgNewProject.h \
    WidgetExplorer.h \
    WidgetOutputLog.h \
    MainWindow.h \
    HyGlobal.h \
    Item.h \
    ItemSprite.h \
    ItemFont.h \
    WidgetGlyphCreator.h \
    WidgetFontMap.h \
    DlgNewItem.h \
    scriptum/imagepacker.h \
    scriptum/maxrects.h \
    WidgetSprite.h \
    DlgInputName.h \
    ItemProject.h \
    WidgetLiveVariable.h \
    WidgetLiveVarManager.h \
    HyGuiRenderer.h \
    DlgAtlasGroupSettings.h \
    WidgetAtlasManager.h \
    WidgetAtlasGroup.h \
    HyGuiDebugger.h \
    DlgSetEngineLocation.h \
    ItemSpriteCmds.h \
    WidgetSpriteState.h \
    HyGuiFrame.h \
    ItemWidget.h

FORMS    += \
    DlgNewProject.ui \
    WidgetExplorer.ui \
    WidgetOutputLog.ui \
    MainWindow.ui \
    WidgetGlyphCreator.ui \
    WidgetFontMap.ui \
    DlgNewItem.ui \
    WidgetSprite.ui \
    DlgInputName.ui \
    WidgetLiveVariable.ui \
    WidgetLiveVarManager.ui \
    DlgAtlasGroupSettings.ui \
    WidgetAtlasManager.ui \
    WidgetAtlasGroup.ui \
    DlgSetEngineLocation.ui \
    WidgetSpriteState.ui

Release:DESTDIR = ../bin/release
Release:OBJECTS_DIR = ../bin/release/.obj
Release:MOC_DIR = ../bin/release/.moc
Release:RCC_DIR = ../bin/release/.rcc
Release:UI_DIR = ../bin/release/.ui

Debug:DESTDIR = ../bin/debug
Debug:OBJECTS_DIR = ../bin/debug/.obj
Debug:MOC_DIR = ../bin/debug/.moc
Debug:RCC_DIR = ../bin/debug/.rcc
Debug:UI_DIR = ../bin/debug/.ui

DEFINES += _HARMONYGUI _HARMONYSINGLETHREAD QT_USE_QSTRINGBUILDER

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../Harmony/bin/x64/ -lHarmonyQt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Harmony/bin/x64/ -lHarmonyQtd

INCLUDEPATH += "$$PWD/../../Harmony/include"
DEPENDPATH += "$$PWD/../../Harmony/include"

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../Harmony/bin/x64/libHarmonyQt.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../Harmony/bin/x64/libHarmonyQtd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../Harmony/bin/x64/HarmonyQt.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../Harmony/bin/x64/HarmonyQtd.lib

win32: LIBS += -lAdvAPI32

RESOURCES += \
    res/DesignerTool.qrc
