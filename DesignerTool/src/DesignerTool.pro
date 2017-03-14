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
    WidgetExplorer.cpp \
    WidgetOutputLog.cpp \
    MainWindow.cpp \
    HyGlobal.cpp \
    Item.cpp \
    ItemSprite.cpp \
    ItemFont.cpp \
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
    HyGuiDebugger.cpp \
    DlgSetEngineLocation.cpp \
    WidgetSpriteState.cpp \
    HyGuiFrame.cpp \
    ItemWidget.cpp \
    WidgetSpriteModelView.cpp \
    WidgetFont.cpp \
    WidgetFontModelView.cpp \
    DlgColorPicker.cpp \
    WidgetFontState.cpp \
    WidgetColor.cpp \
    QtWaitingSpinner/waitingspinnerwidget.cpp \
    WidgetAudioManager.cpp \
    WidgetAudioBank.cpp \
    WidgetAudioModelView.cpp \
    HyGuiWave.cpp \
    WidgetAudio.cpp \
    ItemAudio.cpp \
    WidgetAudioState.cpp \
    WidgetRangedSlider.cpp \
    WidgetFontUndoCmds.cpp \
    WidgetSpriteUndoCmds.cpp \
    WidgetGradientEditor.cpp \
    ItemSpine.cpp \
    WidgetSpine.cpp \
    ItemAtlases.cpp \
    IDraw.cpp \
    SpriteDraw.cpp

HEADERS  += \
    DlgNewProject.h \
    WidgetExplorer.h \
    WidgetOutputLog.h \
    MainWindow.h \
    HyGlobal.h \
    Item.h \
    ItemSprite.h \
    ItemFont.h \
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
    HyGuiDebugger.h \
    DlgSetEngineLocation.h \
    WidgetSpriteState.h \
    HyGuiFrame.h \
    ItemWidget.h \
    WidgetSpriteModelView.h \
    WidgetFont.h \
    WidgetFontModelView.h \
    DlgColorPicker.h \
    WidgetFontState.h \
    WidgetColor.h \
    QtWaitingSpinner/waitingspinnerwidget.h \
    WidgetAudioManager.h \
    WidgetAudioBank.h \
    WidgetAudioModelView.h \
    HyGuiWave.h \
    WidgetAudio.h \
    ItemAudio.h \
    WidgetAudioState.h \
    WidgetRangedSlider.h \
    WidgetUndoCmds.h \
    WidgetFontUndoCmds.h \
    WidgetSpriteUndoCmds.h \
    WidgetGradientEditor.h \
    ItemSpine.h \
    WidgetSpine.h \
    ItemAtlases.h \
    IDraw.h \
    SpriteDraw.h

FORMS    += \
    DlgNewProject.ui \
    WidgetExplorer.ui \
    WidgetOutputLog.ui \
    MainWindow.ui \
    DlgNewItem.ui \
    WidgetSprite.ui \
    DlgInputName.ui \
    WidgetLiveVariable.ui \
    WidgetLiveVarManager.ui \
    DlgAtlasGroupSettings.ui \
    WidgetAtlasManager.ui \
    DlgSetEngineLocation.ui \
    WidgetSpriteState.ui \
    WidgetFont.ui \
    DlgColorPicker.ui \
    WidgetFontState.ui \
    WidgetColor.ui \
    WidgetAudioManager.ui \
    WidgetAudioBank.ui \
    WidgetAudio.ui \
    WidgetAudioState.ui \
    WidgetRangedSlider.ui \
    WidgetGradientEditor.ui \
    WidgetSpine.ui

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
