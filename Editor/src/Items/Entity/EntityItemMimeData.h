/**************************************************************************
*	EntityItemMimeData.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2023 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef EntityItemMimeData_H
#define EntityItemMimeData_H

#include "IMimeData.h"

class ProjectItemData;
class EntityTreeItemData;

class EntityItemMimeData : public IMimeData
{
public:
	EntityItemMimeData(ProjectItemData &entityRef, QList<EntityTreeItemData *> &itemListRef);
	virtual ~EntityItemMimeData();
};

#endif // EntityItemMimeData_H
