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
	ProjectItemData &			m_EntityItemRef;
	QList<ProjectItemData *>	m_ChildrenList;
	QList<EntityTreeItem *>		m_NodeList;

public:
	EntityUndoCmd_AddChildren(ProjectItemData &entityItemRef, QList<ProjectItemData *> projItemList, QUndoCommand *pParent = nullptr) :
		QUndoCommand(pParent),
		m_EntityItemRef(entityItemRef),
		m_ChildrenList(projItemList)
	{
		if(m_EntityItemRef.GetType() != ITEM_Entity)
			HyGuiLog("EntityUndoCmd recieved wrong type: " % QString::number(m_EntityItemRef.GetType()), LOGTYPE_Error);

		setText("Add New Child Node(s)");
	}

	virtual ~EntityUndoCmd_AddChildren()
	{ }

	virtual void redo() override
	{
		QList<ProjectItemData *> itemList;
		for(auto *pProjItem : m_ChildrenList)
		{
			if(static_cast<EntityModel *>(m_EntityItemRef.GetModel())->GetNodeTreeModel().IsItemValid(pProjItem, true))
				itemList.push_back(pProjItem);
		}

		m_NodeList = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_AddNewChildren(itemList, -1);
		m_EntityItemRef.FocusWidgetState(0, -1);
	}

	virtual void undo() override
	{
		for(auto *pNodeItem : m_NodeList)
		{
			if(static_cast<EntityModel *>(m_EntityItemRef.GetModel())->GetNodeTreeModel().IsItemValid(pNodeItem, true))
				static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_RemoveTreeItem(pNodeItem);
		}
		m_NodeList.clear();

		m_EntityItemRef.FocusWidgetState(0, -1);
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_PopChild : public QUndoCommand
{
	ProjectItemData &			m_EntityItemRef;
	EntityTreeItem *			m_pNode;
	uint32						m_uiIndex;

public:
	EntityUndoCmd_PopChild(ProjectItemData &entityItemRef, EntityTreeItem *pNodeItem, QUndoCommand *pParent = nullptr) :
		m_EntityItemRef(entityItemRef),
		m_pNode(pNodeItem),
		m_uiIndex(0)
	{
		if(m_EntityItemRef.GetType() != ITEM_Entity)
			HyGuiLog("EntityUndoCmd recieved wrong type: " % QString::number(m_EntityItemRef.GetType()), LOGTYPE_Error);

		setText("Remove Child Node");
	}

	virtual ~EntityUndoCmd_PopChild()
	{ }

	virtual void redo() override
	{
		m_uiIndex = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_RemoveTreeItem(m_pNode);
		m_EntityItemRef.FocusWidgetState(0, -1);
	}

	virtual void undo() override
	{
		static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_AddChild(m_pNode, m_uiIndex);
		m_EntityItemRef.FocusWidgetState(0, -1);
	}
};

#endif // ENTITYUNDOCMDS_H
