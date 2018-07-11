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
class EntityTreeItem;

enum EntityCmd
{
	ENTITYCMD_AddNewChild = 0,
	ENTITYCMD_AddPrimitive,
};

class EntityUndoCmd : public QUndoCommand
{
	// NOTE: Member order matters!
	const EntityCmd     m_eCMD;
	ProjectItem &       m_ItemRef;
	void *              m_pParameter;

	EntityWidget *      m_pWidget;
	EntityModel *       m_pModel;
	EntityTreeItem *    m_pParentTreeItem;

	int                 m_iRow;

public:
	EntityUndoCmd(EntityCmd eCMD, ProjectItem &itemRef, void *pParameter, QUndoCommand *pParent = 0);
	virtual ~EntityUndoCmd();

	virtual void redo() override;
	virtual void undo() override;
};

#endif // ENTITYUNDOCMDS_H
