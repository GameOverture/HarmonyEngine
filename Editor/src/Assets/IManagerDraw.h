/**************************************************************************
 *	IManagerDraw.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IMANAGERDRAW_H
#define IMANAGERDRAW_H

#include "Global.h"
#include "IDraw.h"

#include <QWidget>

class ProjectItemData;
class TreeModelItemData;
class IAssetItemData;

class IManagerDraw : public IDraw
{
public:
	IManagerDraw();
	virtual ~IManagerDraw();

	virtual void SetHover(TreeModelItemData *pHoverItem) = 0;
	virtual void SetSelected(QList<IAssetItemData *> selectedList) = 0;
};

#endif // IMANAGERDRAW_H
