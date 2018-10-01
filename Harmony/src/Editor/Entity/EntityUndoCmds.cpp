/**************************************************************************
 *	EntityUndoCmds.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "EntityUndoCmds.h"
#include "EntityModel.h"
#include "EntityWidget.h"

EntityUndoCmd::EntityUndoCmd(EntityCmd eCMD, ProjectItem &entityItemRef, void *pParameter, QUndoCommand *pParent /*= 0*/) :		QUndoCommand(pParent),
																																m_eCMD(eCMD),
																																m_EntityItemRef(entityItemRef),
																																m_pWidget(static_cast<EntityWidget *>(entityItemRef.GetWidget())),
																																m_pModel(static_cast<EntityModel *>(entityItemRef.GetModel())),
																																m_pParentTreeItem(static_cast<EntityWidget *>(entityItemRef.GetWidget())->GetCurSelectedTreeItem()),
																																m_iRow(0)
{
	if(m_EntityItemRef.GetType() != ITEM_Entity)
		HyGuiLog("EntityUndoCmd recieved wrong type: " % QString::number(m_EntityItemRef.GetType()) , LOGTYPE_Error);

	switch(m_eCMD)
	{
	case ENTITYCMD_AddNewChild:
		setText("Add New Child");
		break;

	case ENTITYCMD_AddPrimitive:
		setText("Add Primitive");
		if(pParameter == nullptr)
			pParameter = new ProjectItem(m_EntityItemRef.GetProject(), ITEM_Primitive, nullptr, "Primitive", QJsonValue(), false);
		break;
	}

	m_pTreeItem = new EntityTreeItem(static_cast<ProjectItem *>(pParameter));
}

/*virtual*/ EntityUndoCmd::~EntityUndoCmd()
{
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
