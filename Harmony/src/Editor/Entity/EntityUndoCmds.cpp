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

EntityUndoCmd::EntityUndoCmd(EntityCmd eCMD, ProjectItem &itemRef, void *pParameter, QUndoCommand *pParent /*= 0*/) :   QUndoCommand(pParent),
																														m_eCMD(eCMD),
																														m_ItemRef(itemRef),
																														m_pWidget(static_cast<EntityWidget *>(m_ItemRef.GetWidget())),
																														m_pModel(static_cast<EntityModel *>(m_ItemRef.GetModel())),
																														m_pParentTreeItem(m_pWidget->GetCurSelectedTreeItem()),
																														m_iRow(0)
{
	if(m_ItemRef.GetType() != ITEM_Entity)
		HyGuiLog("EntityUndoCmd recieved wrong type: " % QString::number(m_ItemRef.GetType()) , LOGTYPE_Error);

	switch(m_eCMD)
	{
		case ENTITYCMD_AddNewChild: {
			setText("Add New Child");
			m_pParameter = new EntityTreeItem(&m_pModel->GetTreeModel(), static_cast<ProjectItem *>(pParameter));
		} break;

		case ENTITYCMD_AddPrimitive: {
			setText("Add Primitive");
			m_pParameter = new EntityTreeItem(&m_pModel->GetTreeModel(), static_cast<ProjectItem *>(pParameter));
		} break;
	}
}

/*virtual*/ EntityUndoCmd::~EntityUndoCmd()
{
}

/*virtual*/ void EntityUndoCmd::redo() /*override*/
{
	switch(m_eCMD)
	{
		case ENTITYCMD_AddNewChild: {
			m_pModel->GetTreeModel().InsertItem(m_iRow, static_cast<EntityTreeItem *>(m_pParameter), m_pParentTreeItem);
		} break;

		case ENTITYCMD_AddPrimitive: {
		} break;
	}
}

/*virtual*/ void EntityUndoCmd::undo() /*override*/
{
	switch(m_eCMD)
	{
		case ENTITYCMD_AddNewChild: {
			m_iRow = static_cast<EntityTreeItem *>(m_pParameter)->GetRow();
			m_pModel->GetTreeModel().RemoveItems(m_iRow, 1, m_pParentTreeItem);
		} break;

		case ENTITYCMD_AddPrimitive: {

		} break;
	}
}
