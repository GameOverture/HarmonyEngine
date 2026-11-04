/**************************************************************************
 *	main.cpp
 *
 *	Harmony Engine - HyEditor
 *	Copyright (c) 2016 Jason Knobler
 *
 *	GPL/LGPL License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
 *************************************************************************/
#include "Global.h"
#include "MainWindow.h"
#include "AtlasFrame.h"
#include "TreeModelItemData.h"
#include "DlgProjectSettings.h"

#include <QApplication>

int main(int argc, char *argv[])
{
	//QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication a(argc, argv);

	//qRegisterMetaTypeStreamOperators<TreeModelItemData *>("TreeModelItemData *");

	HyGlobal::Initialize();

	MainWindow w;
	w.show();

	return a.exec();
}
