/**************************************************************************
 *	EntityUndoCmds.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "EntityUndoCmds.h"
#include "EntityModel.h"
#include "EntityWidget.h"

EntityUndoCmd::EntityUndoCmd(EntityCmd eCMD, ProjectItem &entityItemRef, ExplorerItem *pParameter, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_eCMD(eCMD),
	m_EntityItemRef(entityItemRef)
{
	if(m_EntityItemRef.GetType() != ITEM_Entity)
		HyGuiLog("EntityUndoCmd recieved wrong type: " % QString::number(m_EntityItemRef.GetType()) , LOGTYPE_Error);

	m_EntityItemRef.

	switch(m_eCMD)
	{
	case ENTITYCMD_AddNewChild:
		setText("Add New Child");
		m_pTreeItem = new EntityTreeItem(pParameter);
		break;

	case ENTITYCMD_AddPrimitive:
		setText("Add Primitive");
		m_pTreeItem = new EntityTreeItem(pParameter);
		break;
	}
}

/*virtual*/ EntityUndoCmd::~EntityUndoCmd()
{
	delete m_pTreeItem;
}

/*virtual*/ void EntityUndoCmd::redo() /*override*/
{
	switch(m_eCMD)
	{
	case ENTITYCMD_AddNewChild:
	case ENTITYCMD_AddPrimitive:
		m_pModel->InsertTreeItem(m_iRow, m_pTreeItem, m_pParentTreeItem);
		break;
	}
}

/*virtual*/ void EntityUndoCmd::undo() /*override*/
{
	switch(m_eCMD)
	{
	case ENTITYCMD_AddNewChild:
	case ENTITYCMD_AddPrimitive:
		m_iRow = m_pTreeItem->GetRow();
		m_pModel->RemoveTreeItems(m_iRow, 1, m_pParentTreeItem);
		break;
	}
}
