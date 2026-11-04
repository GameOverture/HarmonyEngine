/**************************************************************************
 *	IManagerDraw.cpp
 *
 *	Harmony Engine - HyEditor
 *	Copyright (c) 2020 Jason Knobler
 *
 *	GPL/LGPL License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
 *************************************************************************/
#include "Global.h"
#include "IManagerDraw.h"
#include "ProjectItemData.h"
#include "IModel.h"
#include "MainWindow.h"
#include "Harmony.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QKeyEvent>
#include <QApplication>

#define KEY_PanCamera Qt::Key_Space

IManagerDraw::IManagerDraw() :
	IDraw(nullptr, FileDataPair())
{
}

/*virtual*/ IManagerDraw::~IManagerDraw()
{
}

