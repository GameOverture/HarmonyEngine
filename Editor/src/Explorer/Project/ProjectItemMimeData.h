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

	// If from another project, OR eDropAction is COPY, it will modify 'jsonDataOut' and regenerate all UUIDs
	static void RegenUuids(Project *pDestProject, Qt::DropAction eDropAction, QByteArray &jsonDataOut);
};

#endif // ProjectItemMimeData_H
