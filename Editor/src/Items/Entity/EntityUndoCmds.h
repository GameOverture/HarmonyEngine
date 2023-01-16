/**************************************************************************
 *	EntityUndoCmds.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2022 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ENTITYUNDOCMDS_H
#define ENTITYUNDOCMDS_H

#include "EntityModel.h"

#include <QUndoCommand>

class EntityUndoCmd_AddChildren : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	QList<ProjectItemData *>		m_ChildrenList;
	QList<EntityTreeItemData *>		m_NodeList;

public:
	EntityUndoCmd_AddChildren(ProjectItemData &entityItemRef, QList<ProjectItemData *> projItemList, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_AddChildren();

	virtual void redo() override;
	virtual void undo() override;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_PopChild : public QUndoCommand
{
	ProjectItemData &			m_EntityItemRef;
	EntityTreeItemData *		m_pNode;
	uint32						m_uiIndex;

public:
	EntityUndoCmd_PopChild(ProjectItemData &entityItemRef, EntityTreeItemData *pNodeItem, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_PopChild();

	virtual void redo() override;
	virtual void undo() override;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_SelectionChanged : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	QList<EntityTreeItemData *>		m_SelectedItemDataList;
	QList<EntityTreeItemData *>		m_DeselectedItemDataList;

public:
	EntityUndoCmd_SelectionChanged(ProjectItemData &entityItemRef, QList<EntityTreeItemData *> selectedItemDataList, QList<EntityTreeItemData *> deselectedItemDataList, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_SelectionChanged();

	virtual void redo() override;
	virtual void undo() override;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // ENTITYUNDOCMDS_H
