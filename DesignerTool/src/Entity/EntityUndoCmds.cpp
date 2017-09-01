/**************************************************************************
 *	EntityUndoCmds.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "EntityUndoCmds.h"
#include "EntityTreeModel.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EntityUndoCmd_AddNewChild::EntityUndoCmd_AddNewChild(EntityTreeItem *pParentTreeItem, EntityTreeModel *pTreeModel, ProjectItem *pItem, QUndoCommand *pParent /*= 0*/) : QUndoCommand(pParent),
                                                                                                                                                                        m_pParentTreeItem(pParentTreeItem),
                                                                                                                                                                        m_pTreeModel(pTreeModel),
                                                                                                                                                                        m_pItem(pItem),
                                                                                                                                                                        m_pNewTreeItem(new EntityTreeItem(m_pTreeModel, m_pItem)),
                                                                                                                                                                        m_iRow(0)
{
    setText("Add New Child");
}

/*virtual*/ EntityUndoCmd_AddNewChild::~EntityUndoCmd_AddNewChild()
{
}

void EntityUndoCmd_AddNewChild::redo()
{
    m_pTreeModel->InsertItem(m_iRow, m_pNewTreeItem, m_pParentTreeItem);
}

void EntityUndoCmd_AddNewChild::undo()
{
    m_iRow = m_pNewTreeItem->GetRow();
    m_pTreeModel->RemoveItems(m_iRow, 1, m_pParentTreeItem);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
