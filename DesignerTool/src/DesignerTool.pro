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
    HyApp.cpp \
    DlgNewProject.cpp \
    WidgetExplorer.cpp \
    WidgetOutputLog.cpp \
    WidgetRenderer.cpp \
    HyGfx.cpp \
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
    WidgetAtlas.cpp \
    WidgetSprite.cpp \
    DlgInputName.cpp \
    ItemProject.cpp \
    WidgetLiveVariable.cpp \
    WidgetLiveVarManager.cpp \
    HyTcpServer.cpp \
    HyGuiTexture.cpp \
    DlgOpenProject.cpp

HEADERS  += \
    HyApp.h \
    DlgNewProject.h \
    WidgetExplorer.h \
    WidgetOutputLog.h \
    WidgetRenderer.h \
    HyGfx.h \
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
    WidgetAtlas.h \
    WidgetSprite.h \
    DlgInputName.h \
    ItemProject.h \
    WidgetLiveVariable.h \
    WidgetLiveVarManager.h \
    HyTcpServer.h \
    HyGuiTexture.h \
    DlgOpenProject.h

FORMS    += \
    DlgNewProject.ui \
    WidgetExplorer.ui \
    WidgetOutputLog.ui \
    WidgetRenderer.ui \
    MainWindow.ui \
    WidgetGlyphCreator.ui \
    WidgetFontMap.ui \
    DlgNewItem.ui \
    WidgetAtlas.ui \
    WidgetSprite.ui \
    DlgInputName.ui \
    WidgetLiveVariable.ui \
    WidgetLiveVarManager.ui \
    DlgOpenProject.ui

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
