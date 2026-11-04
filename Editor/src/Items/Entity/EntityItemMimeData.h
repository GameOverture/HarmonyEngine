/**************************************************************************
*	EntityItemMimeData.h
*
*	Harmony Engine - HyEditor
*	Copyright (c) 2023 Jason Knobler
*
*	GPL/LGPL License:
*	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityFrameMimeData : public IMimeData
{
public:
	EntityFrameMimeData(QJsonObject serializedKeyFramesObj);
	virtual ~EntityFrameMimeData();

	bool IsValidForPaste() const;
};

#endif // EntityItemMimeData_H
