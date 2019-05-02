/**************************************************************************
 *	EntityUndoCmds.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ENTITYUNDOCMDS_H
#define ENTITYUNDOCMDS_H

#include <QUndoCommand>

class ProjectItem;
class EntityWidget;
class EntityModel;

enum EntityCmd
{
	ENTITYCMD_AddNewChild = 0,
	ENTITYCMD_AddPrimitive,
};

class EntityUndoCmd : public QUndoCommand
{
	const EntityCmd		m_eCMD;
	ProjectItem &		m_EntityItemRef;

public:
	EntityUndoCmd(EntityCmd eCMD, ProjectItem &entityItemRef, ExplorerItem *pParameter, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd();

	virtual void redo() override;
	virtual void undo() override;
};

#endif // ENTITYUNDOCMDS_H
