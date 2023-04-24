/**************************************************************************
*	ProjectItemMimeData.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef ProjectItemMimeData_H
#define ProjectItemMimeData_H

#include "IMimeData.h"

class ExplorerItemData;
class ProjectItemData;

class ProjectItemMimeData : public IMimeData
{
public:
	ProjectItemMimeData(QList<ExplorerItemData *> &itemListRef);
	virtual ~ProjectItemMimeData();

	static void RegenUuids(Project *pDestProject, QByteArray &jsonDataOut);
};

#endif // ProjectItemMimeData_H
