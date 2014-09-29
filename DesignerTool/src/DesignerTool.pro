#-------------------------------------------------
#
# Project created by QtCreator 2014-07-12T14:10:09
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DesignerTool
TEMPLATE = app


SOURCES += main.cpp \
    HyApp.cpp \
    Spine.cpp \
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
    WidgetAtlases.cpp

HEADERS  += \
    HyApp.h \
    Spine.h \
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
    WidgetAtlases.h

FORMS    += \
    DlgNewProject.ui \
    WidgetExplorer.ui \
    WidgetOutputLog.ui \
    WidgetRenderer.ui \
    MainWindow.ui \
    WidgetGlyphCreator.ui \
    WidgetFontMap.ui \
    DlgNewItem.ui \
    WidgetAtlases.ui

DEFINES += _HARMONYGUI _HARMONYSINGLETHREAD QT_USE_QSTRINGBUILDER

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../Harmony/bin/vs2010/ -lHarmonyQt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Harmony/bin/vs2010/ -lHarmonyQtd

INCLUDEPATH += "$$PWD/../../Harmony/include"
DEPENDPATH += "$$PWD/../../Harmony/include"

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../Harmony/bin/vs2010/libHarmonyQt.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../Harmony/bin/vs2010/libHarmonyQtd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../Harmony/bin/vs2010/HarmonyQt.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../Harmony/bin/vs2010/HarmonyQtd.lib

win32: LIBS += -lAdvAPI32

RESOURCES += \
    res/DesignerTool.qrc
