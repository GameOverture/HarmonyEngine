/**************************************************************************
 *	main.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "MainWindow.h"
#include <QApplication>

#include "HyGuiGlobal.h"
#include "AtlasFrame.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaTypeStreamOperators<HyGuiFrame *>("HyGuiFrame *");

    HyGlobal::Initialize();

    MainWindow w;
    w.show();

    return a.exec();
}
