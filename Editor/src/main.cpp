/**************************************************************************
 *	main.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "MainWindow.h"
#include "AtlasFrame.h"
#include "TreeModelItemData.h"
#include "DlgProjectSettings.h"

#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication a(argc, argv);

	//qRegisterMetaTypeStreamOperators<TreeModelItemData *>("TreeModelItemData *");

	HyGlobal::Initialize();

	MainWindow w;
	w.show();

	return a.exec();
}
